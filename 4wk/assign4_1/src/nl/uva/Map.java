package nl.uva;

import java.io.IOException;
import java.util.StringTokenizer;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;

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
        StringTokenizer itr = new StringTokenizer(value.toString()
                .replaceAll("[^a-zA-Z0-9#]", " "));
        int count = 0;
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
}
