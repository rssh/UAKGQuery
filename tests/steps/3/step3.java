import java.io.*;
import java.util.*;

import org.omg.CORBA.*;
import org.omg.CosQuery.*;
import org.omg.CORBA.ORBPackage.*;
import org.omg.CosQueryCollection.*;

import ua.kiev.gradsoft.RC.*;
import ua.kiev.gradsoft.UAKGQuery.*;
    
public class step3
{
    private static ORB orb;
    public static void main(String args[])
    {
        try
        {
            Properties props = System.getProperties();
            props.put("org.omg.CORBA.ORBClass", "com.ooc.CORBA.ORB");
            props.put("org.omg.CORBA.ORBSingletonClass", "com.ooc.CORBA.ORBSingleton");
            System.setProperties(props);
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

            UAKGQueryManager queryManager = null;
            try 
            {
                queryManager = dbcm.createQueryManager("scott","tiger","","Oracle8","");
            }
            catch(QueryManagerNotFound ex)
            {
                System.out.println("Can't find query manager.");
                return;
            }
            printLib.orb = orb;
            run_query(queryManager);
            System.out.println("disconnecting.");
        }
        catch(SystemException ex)
        {
            System.out.println("SystemException: " + ex);
            return;
        }
        System.out.println("Success!!!");
    }
    private static void run_query(UAKGQueryManager queryManager)
    {
        String query_text = "SELECT * FROM tab";
        RecordDescriptionHolder emptyRecordDescriptionHolder = new RecordDescriptionHolder();
        emptyRecordDescriptionHolder.value = new FieldDescription[0];
        RecordSeqHolder emptyRecordSeqHolder = new RecordSeqHolder();
        emptyRecordSeqHolder.value = new FieldValue[0][0];
        try 
        {
            UAKGQuery query = queryManager.create_query(query_text, 0);
            query.prepare_query(emptyRecordDescriptionHolder.value);
            query.execute_records(emptyRecordSeqHolder.value);
            int nRetrieved = 10;
            BooleanHolder more = new BooleanHolder(true);
            RecordSeqHolder recordSeqHolder = new RecordSeqHolder();
            while(more.value) 
            {
                recordSeqHolder.value = query.fetch_records(nRetrieved, more);
                printLib.printRecordSeq(System.out, recordSeqHolder);
            }
            query.destroy();
        }
        catch(QueryTypeInvalid ex)
        {
            System.out.println("QueryTypeInvalid");
        }
        catch(QueryInvalid ex)
        {
            System.out.println("QueryInvalid");
        }
        catch(QueryProcessingError ex)
        {
            System.out.println("QueryProcessingError");
            System.out.println(ex.why);
        }
    }
}
