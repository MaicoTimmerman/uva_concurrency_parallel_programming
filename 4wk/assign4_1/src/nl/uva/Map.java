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
    public void map(LongWritable key, Text value,
            OutputCollector<Text, IntWritable> oc, Reporter rprtr)
    throws IOException {

        int count = 0;
        String tweet = value.toString();

        /* Don't process empty strings */
        /* Only check the actual tweet for hashtags. */
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

                /* Send the result to the Reducer */
                sendword.set(word.toString().toLowerCase());
                oc.collect(sendword, one);
                rprtr.incrCounter(Counters.INPUT_LINES, 1);

                count++;
                if ((++count % 100) == 0) {
                    rprtr.setStatus("Finished processing " + count + " records");
                }
            }
        }
    }
}
