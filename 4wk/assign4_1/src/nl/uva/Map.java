package nl.uva;

import java.io.IOException;
import java.util.StringTokenizer;
import me.champeau.ld.UberLanguageDetector;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;
import edu.stanford.nlp.ling.CoreAnnotations;
import edu.stanford.nlp.pipeline.Annotation;
import edu.stanford.nlp.pipeline.StanfordCoreNLP;
import edu.stanford.nlp.rnn.RNNCoreAnnotations;
import edu.stanford.nlp.sentiment.SentimentCoreAnnotations;
import edu.stanford.nlp.trees.Tree;
import edu.stanford.nlp.util.CoreMap;

/**
 * Word count mapper.
 *
 * @author S. Koulouzis
 */
public class Map extends MapReduceBase implements Mapper<LongWritable, Text, Text, IntWritable> {

    Log log = LogFactory.getLog(Map.class);
    private final static IntWritable one = new IntWritable(1);
    private Text word = new Text();
    private Text sendword = new Text();

    static enum Counters {

        INPUT_LINES
    }

    @Override
    public void map(LongWritable key, Text value, OutputCollector<Text, IntWritable> oc, Reporter rprtr) throws IOException {

        int count = 0;

        String tweet = value.toString();
        if (tweet.substring(0,1).matches("W")) {
            tweet = tweet.substring(2);
            String lang = UberLanguageDetector.getInstance().detectLang(tweet);

            System.out.println(tweet);

            if (lang.equals("en")) {
                /* int sent = findSentiment(tweet); */
            }
        }

        /* Remove all the non-alphanumeric characters from the sentence */
        StringTokenizer itr = new StringTokenizer(value.toString()
                .replaceAll("[^a-zA-Z0-9#]", " "));

        /* Loop through all the words */
        while (itr.hasMoreTokens()) {
            word.set(itr.nextToken());
            if (!(word.toString().toLowerCase().matches("#\\w*[a-zA-Z]+\\w*"))) {
                continue;
            }
            sendword.set(word.toString().toLowerCase());
            oc.collect(sendword, one);
            rprtr.incrCounter(Counters.INPUT_LINES, 1);
            count++;
            if ((++count % 100) == 0) {
                rprtr.setStatus("Finished processing " + count + " records");
            }
        }
    }


    /**
     * Find the sentiment of a tweet.
     *
     * @param text
     * @return
     */
    private int findSentiment(String text) {

        Properties props = new Properties();
        props.setProperty("annotators", "tokenize, ssplit, parse, sentiment");
        props.put("parse.model", parseModelPath);
        props.put("sentiment.model", sentimentModelPath);
        StanfordCoreNLP pipeline = new StanfordCoreNLP(props);
        int mainSentiment = 0;

        if (text != null && text.length() > 0) {
            int longest = 0;
            Annotation annotation = pipeline.process(text);

            for (CoreMap sentence : annotation
                    .get(CoreAnnotations.SentencesAnnotation.class))
            {
                Tree tree = sentence
                    .get(SentimentCoreAnnotations.AnnotatedTree.class);
                int sentiment = RNNCoreAnnotations.getPredictedClass(tree);
                String partText = sentence.toString();

                if (partText.length() > longest) {
                    mainSentiment = sentiment;
                    longest = partText.length();
                }
            }
        }
    }
}
