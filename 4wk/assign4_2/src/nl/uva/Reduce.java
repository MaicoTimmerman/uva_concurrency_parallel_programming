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
 * Further calculation includes standard deviation and number of times
 * the hashtag is present in the dataset.
 *
 * @author S. Koulouzis
 * @author R. Klusman (10675671)
 * @author M. Timmerman (10542590)
 */
public class Reduce extends MapReduceBase
    implements Reducer<Text, IntWritable, Text, Text> {

    @Override
    public void reduce(Text key, Iterator<IntWritable> itrtr,
            OutputCollector<Text, Text> output, Reporter rprtr)
    throws IOException {

        int sum = 0;
        int count = 0;
        double mean = 0;
        double stddev = 0;

        /* Save the values to calculate the std deviation */
        ArrayList<Integer> cache = new ArrayList<Integer>();

        /* Calculate the mean */
        while (itrtr.hasNext()) {
            int value = itrtr.next().get();
            sum += value;
            count++;
            cache.add(value);
        }
        mean = (double)sum / count;

        /* Calculate the std deviation */
        for (int value : cache) {
            System.out.println("value: "+value);
            stddev = stddev + Math.pow(value - mean, 2);
        }
        stddev = stddev / count;
        stddev = Math.sqrt(stddev);

        /* Return the values */
        output.collect(
                new Text("\""+key.toString()),
                new Text("("+count+")\" "+mean+" "+stddev)
                );
    }
}
