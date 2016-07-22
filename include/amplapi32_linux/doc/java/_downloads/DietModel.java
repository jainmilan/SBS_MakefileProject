import java.io.IOException;

import com.ampl.AMPL;
import com.ampl.DataFrame;

public class DietModel {

    public static void main(String[] args) throws IOException {

        AMPL ampl = new AMPL();
        // Outer try-catch-finally block, to be sure of releasing the AMPL
        // object when done
        try {
            if (args.length > 0)
                ampl.setOption("solver", args[0]);

            // Use the provided path or the default one
            String modelDirectory = args.length > 1 ?
                args[1] : "../models/diet";
            // Load the AMPL model from file
            ampl.read(modelDirectory + "/diet.mod");

            String[] foods = new String[] { "BEEF", "CHK", "FISH", "HAM",
                    "MCH", "MTL", "SPG", "TUR" };

            DataFrame df = new DataFrame(1, "FOOD");
            df.setColumn("FOOD", foods);
            df.addColumn("cost", new double[] { 3.59, 2.59, 2.29, 2.89, 1.89,
                    1.99, 1.99, 2.49 });
            df.addColumn("f_min");
            df.addColumn("f_max");
            for (int i = 0; i < foods.length; i++) {
                df.setValue(foods[i], "f_min", 2);
                df.setValue(foods[i], "f_max", 10);
            }
            ampl.setData(df, "FOOD");

            String[] nutrients = new String[] { "A", "C", "B1", "B2", "NA",
                    "CAL" };
            df = new DataFrame(1, "NUTR");
            df.setColumn("NUTR", nutrients);
            df.addColumn("n_min", new double[] { 700, 700, 700, 700, 0, 16000 });
            df.addColumn("n_max", new double[] { 20000, 20000, 20000, 20000,
                    50000, 24000 });
            ampl.setData(df, "NUTR");

            double[][] amounts = new double[6][];
            amounts[0] = new double[] { 60, 8, 8, 40, 15, 70, 25, 60 };
            amounts[1] = new double[] { 20, 0, 10, 40, 35, 30, 50, 20 };
            amounts[2] = new double[] { 10, 20, 15, 35, 15, 15, 25, 15 };
            amounts[3] = new double[] { 15, 20, 10, 10, 15, 15, 15, 10 };
            amounts[4] = new double[] { 928, 2180, 945, 278, 1182, 896, 1329,
                    1397 };
            amounts[5] = new double[] { 295, 770, 440, 430, 315, 400, 379, 450 };

            df = new DataFrame(2, "NUTR", "FOOD", "amt");
            df.setMatrix(amounts, nutrients, foods);
            ampl.setData(df);

            ampl.solve();

            System.out.println(ampl.getObjective("total_cost").value());
        } finally {
            ampl.close();
        }
    }
}
