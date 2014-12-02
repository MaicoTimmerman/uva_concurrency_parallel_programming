package nl.uva;

import java.io.IOException;
import java.util.Properties;
import java.util.StringTokenizer;
import java.util.logging.*;
import me.champeau.ld.UberLanguageDetector;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.filecache.DistributedCache;
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
public class Map extends MapReduceBase
    implements Mapper<LongWritable, Text, Text, IntWritable> {

    Log log = LogFactory.getLog(Map.class);
    private Text word = new Text();
    private Path[] cachedFiles;
    private Path parseModelPath;
    private Path sentimentModelPath;

    @Override
    public void configure(JobConf conf) {
        try {
            cachedFiles = DistributedCache.getLocalCacheFiles(conf);
            parseModelPath = cachedFiles[0];
            sentimentModelPath = cachedFiles[1];
        } catch (Exception ex) {
            Logger.getLogger(Map.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    @Override
    public void map(LongWritable key, Text value,
            OutputCollector<Text, IntWritable> oc, Reporter rprtr)
    throws IOException {

        IntWritable sentiment;

        String tweet = value.toString();

        /* Check for empty strings */
        /* Only process the actual tweet, not the date or the URL */
        if ((!tweet.isEmpty()) && (tweet.substring(0,1).matches("W"))) {

            /* Remove all the non-alphanumeric characters from the sentence */
            StringTokenizer itr = new StringTokenizer(tweet.replaceAll("[^a-zA-Z0-9#_]", " "));

            /* Loop through all the words */
            while (itr.hasMoreTokens()) {
                word.set(itr.nextToken());

                /* Test if a hashtag is not present in the current sentence. */
                if (!(word.toString().toLowerCase().matches("#\\w*[a-zA-Z]+\\w*"))) {
                    continue;
                }

                String lang = UberLanguageDetector.getInstance()
                    .detectLang(tweet);

                /* Check if language is English */
                if (lang.equals("en")) {
                    sentiment = new IntWritable(findSentiment(tweet));

                    /* Send the tweet and the sentiment value to reduce */
                    oc.collect(new Text(word.toString().toLowerCase()), sentiment);
                }
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
        props.put("parse.model", parseModelPath.toString());
        props.put("sentiment.model", sentimentModelPath.toString());
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
        return mainSentiment;
    }
}
