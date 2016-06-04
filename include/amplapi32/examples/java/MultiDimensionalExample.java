import com.ampl.AMPL;
import com.ampl.DataFrame;
import com.ampl.Tuple;

public class MultiDimensionalExample {

    public static void main(String[] args) {

        AMPL ampl = new AMPL();
        // Outer try-catch-finally block, to be sure of releasing the AMPL
        // object when done
        try {
            ampl.eval("set CITIES; set LINKS within (CITIES cross CITIES);");
            ampl.eval("param cost {LINKS} >= 0; param capacity {LINKS} >= 0;");
            ampl.eval("data; set CITIES := PITT NE SE BOS EWR BWI ATL MCO;");

            double[] cost = new double[] { 2.5, 3.5, 1.7, 0.7, 1.3, 1.3, 0.8,
                    0.2, 2.1 };
            double[] capacity = new double[] { 250, 250, 100, 100, 100, 100,
                    100, 100, 100 };

            Object[] links = new Object[] { new Tuple("PITT", "NE"),
                    new Tuple("PITT", "SE"), new Tuple("NE", "BOS"),
                    new Tuple("NE", "EWR"), new Tuple("NE", "BWI"),
                    new Tuple("SE", "EWR"), new Tuple("SE", "BWI"),
                    new Tuple("SE", "ATL"), new Tuple("SE", "MCO") };

            DataFrame df = new DataFrame(1, "LINKS", "cost", "capacity");
            df.setColumn("LINKS", links);
            df.setColumn("cost", cost);
            df.setColumn("capacity", capacity);
            System.out.println(df.toString());
            ampl.setData(df, "LINKS");
        } finally {
            ampl.close();
        }
    }
}
