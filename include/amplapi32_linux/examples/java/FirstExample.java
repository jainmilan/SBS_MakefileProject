import java.io.IOException;

import com.ampl.AMPL;
import com.ampl.DataFrame;
import com.ampl.Objective;
import com.ampl.Parameter;
import com.ampl.Variable;

public class FirstExample {

    public static void main(String[] args) throws IOException {
        // Create an AMPL instance
        AMPL ampl = new AMPL();
        // Embed everything in a try-catch-finally block, so that
        // we are certain that ampl.close() is called to free
        // resources at the end of the execution
        try {
            if (args.length > 0)
                ampl.setOption("solver", args[0]);

            String modelDirectory = args.length > 1 ?
                args[1] : "../models/diet";

            // Interpret the two files
            ampl.read(modelDirectory + "/diet.mod");
            ampl.readData(modelDirectory + "/diet.dat");

            // Solve
            ampl.solve();

            // Get objective entity by AMPL name
            Objective totalcost = ampl.getObjective("total_cost");
            // Print it
            System.out.format("ObjectiveInstance is: %f%n", totalcost.value());

            // Reassign data - specific instances
            Parameter cost = ampl.getParameter("cost");
            cost.setValues(new Object[] { "BEEF", "HAM" }, new double[] { 5.01,
                    4.55 });
            System.out.println("Increased costs of beef and ham.");

            // Resolve and display objective
            ampl.solve();
            System.out.format("New objective value: %f%n", totalcost.value());

            // Reassign data - all instances
            cost.setValues(new double[] { 3, 5, 5, 6, 1, 2, 5.01, 4.55 });
            System.out.println("Updated all costs");
            // Resolve and display objective
            ampl.solve();
            System.out.format("New objective value: %f%n", totalcost.value());

            // Get the values of the variable Buy in a dataframe object
            Variable buy = ampl.getVariable("Buy");
            DataFrame df = buy.getValues();
            // Print them
            System.out.println(df);

            // Get the values of an expression into a DataFrame object
            DataFrame df2 = ampl.getData("{j in FOOD} 100*Buy[j]/Buy[j].ub");
            // Print them
            System.out.println(df2);

        } finally {
            ampl.close();
        }
    }
}
