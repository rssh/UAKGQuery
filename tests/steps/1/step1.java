import org.omg.CORBA.*;
import org.omg.CORBA.ORBPackage.*;
import ua.kiev.gradsoft.UAKGQuery.*;

public class step1
{
    public static void main(String args[])
    {
        try
        {
            ORB orb = ORB.init(args, props);
            org.omg.CORBA.Object obj = null;
            DBConnectionManager dbcm = null;
            try
            {
                obj = orb.resolve_initial_references("UAKGQueryService");
            }
            catch(InvalidName ex)
            {
                System.out.println("UAKGQueryService service is not available");
                return;
            }
            if(obj == null)
            {
                System.out.println("UAKGQueryService object reference is invalid");
                return;
            }
            dbcm = DBConnectionManagerHelper.narrow(obj);
            if(dbcm == null)
            {
                System.out.println("Can't narrow DBConnection to correct type");
                return;
            }                                              
            UAKGQueryManager queryManager;
            try 
            {
                queryManager = dbcm.createQueryManager("scott", "tiger", "", "Oracle8", "");
            }
            catch(QueryManagerNotFound ex)
            {
                System.out.println("Can't find query manager.");
                return;
            }
        }
        catch(SystemException ex)
        {
            System.out.println("SystemException" + ex.getMessage());
            return;
        }
        System.out.println("Success!!!");
    }
}
