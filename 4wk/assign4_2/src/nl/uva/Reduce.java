package nl.uva;

import java.io.IOException;
import java.util.Iterator;
import java.util.ArrayList;
import me.champeau.ld.UberLanguageDetector;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;
import edu.stanford.nlp.ling.CoreAnnotations;
import edu.stanford.nlp.pipeline.Annotation;
import edu.stanford.nlp.pipeline.StanfordCoreNLP;
import edu.stanford.nlp.rnn.RNNCoreAnnotations;
import edu.stanford.nlp.sentiment.SentimentCoreAnnotations;
import edu.stanford.nlp.trees.Tree;
import edu.stanford.nlp.util.CoreMap;

/**
 * Self implemented Reduce class to calculate the average sentiment.
 * The standard deviation is also returned.
 *
 * @author S. Koulouzis
 * @author M. Timmerman
 * @author R. Klusman
 */
public class Reduce extends MapReduceBase
    implements Reducer<IntWritable, IntWritable, Text, DoubleWritable> {

    @Override
    public void reduce(IntWritable key, Iterator<IntWritable> itrtr,
            OutputCollector<Text, DoubleWritable> output, Reporter rprtr)
    throws IOException {

        int sum = 0;
        int count = 0;
        double mean = 0;
        double stddev = 0;

        /* Save the values to calculate the std deviation */
        ArrayList<IntWritable> cache = new ArrayList<IntWritable>();

        /* Calculate the mean */
        while (itrtr.hasNext()) {
            IntWritable value = itrtr.next();
            sum += value.get();
            count++;
            cache.add(value);
        }
        mean = (double)sum / count;

        /* Calculate the std deviation */
        for (IntWritable value : cache) {
            stddev = stddev + Math.pow(value.get() - mean, 2);
        }
        stddev = stddev / mean;
        stddev = Math.sqrt(stddev);

        /* Return the values */
        output.collect(new Text("Stddev: "), new DoubleWritable(stddev));
        output.collect(new Text("Mean: "), new DoubleWritable(mean));
    }
}
