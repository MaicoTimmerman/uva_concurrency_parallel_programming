package nl.uva;

import java.io.IOException;
import java.util.Iterator;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;

/**
 *
 *
 * @author S. Koulouzis
 */
public class Reduce extends MapReduceBase implements Reducer<Text, IntWritable, Text, IntWritable> {

    static enum Counters {

        OUTPUT_LINES
    }

    @Override
    public void reduce(Text key, Iterator<IntWritable> itrtr, OutputCollector<Text, IntWritable> output, Reporter rprtr) throws IOException {

        int sum = 0;
        int count = 0;
        while (itrtr.hasNext()) {
            sum += itrtr.next().get();
            count++;
            if ((++count % 100) == 0) {
                rprtr.setStatus("Finished processing " + count + " records ");
            }
        }
        output.collect(key, new IntWritable(sum));
        rprtr.incrCounter(Counters.OUTPUT_LINES, 1);
    }
}
