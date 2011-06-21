import java.sql.*;

public class perf1_jdbc
{
    private static final int maxNRuns = 3;
    private static final long maxNRetrieved = 100;
    public static void main(String args[])
    {
        try
        {
            Class.forName("oracle.jdbc.driver.OracleDriver"); 
            String url = "jdbc:oracle:thin:@(description=(address_list=(address=(protocol=tcp)(host=10.0.1.253)(port=1521))) (connect_data=(sid=grad)))";
            Class.forName("oracle.jdbc.driver.OracleDriver");
            Connection connection =  DriverManager.getConnection(url, "intbill", "intbill");
            run_query(connection);
            connection.close();
            
            System.out.println("Congratulations! You've got success!");
        }
        catch(Exception ex)
        {
            System.out.println("Sorry! There is an error.");
            ex.printStackTrace(System.out);
        }
    }
    static long diff_ftime(java.util.Date x, java.util.Date y)
    {
        return x.getTime() - y.getTime();
    }
    static void run_query(Connection connection)
    {
        String sqlQuery = "SELECT login, host_ip FROM seances WHERE rownum < 10000";
        long deltaTime[] = new long[maxNRuns];
        double avg_delta;

        java.util.Date frsTime = new java.util.Date();
        java.util.Date sndTime = new java.util.Date();
        try 
        {
            for(int nRetrieved = 0; nRetrieved < maxNRetrieved; nRetrieved++) 
            {
                avg_delta = 0;
                for(int nRuns = 0; nRuns < maxNRuns; nRuns++) 
                {
                    frsTime.setTime(System.currentTimeMillis());
                    Statement statement = connection.createStatement();
                    ResultSet rs = statement.executeQuery(sqlQuery);
                    while(rs.next())
                    {
                        String login = rs.getString("login");
                        String host_ip = rs.getString("host_ip");
                        //System.out.println(i + "\tlogin = \t" + login + "\t host_ip = \t" + host_ip);
                    }
                    statement.close();
                    sndTime.setTime(System.currentTimeMillis());
                    deltaTime[nRuns] = diff_ftime(sndTime, frsTime);      
                    //System.out.println("(i,deltat_t) = " + nRetrieved + "," + deltaTime[nRuns]);
                    avg_delta += deltaTime[nRuns];
                }
                avg_delta /= maxNRuns;
                System.out.println(nRetrieved + "," + avg_delta);
            }
        }
        catch(Exception ex)
        {
           System.out.println("Exception: " + ex);
        }    
    }
}
