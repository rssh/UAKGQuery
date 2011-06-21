import java.io.*;
import java.util.*;

import org.omg.CORBA.*;
import org.omg.CosQuery.*;
import org.omg.CORBA.ORBPackage.*;
import org.omg.CosQueryCollection.*;

import ua.kiev.grad.RC.*;
import ua.kiev.grad.CosQuery.*;
import ua.kiev.grad.CosQuery.RecordDescriptionHolder;
import ua.kiev.grad.CosQuery.RecordSeqHolder;
import ua.kiev.grad.CosQuery.FieldDescription;
import ua.kiev.grad.CORBAView.*;    

public class step4
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
                queryManager = dbcm.createQueryManager("scott","tiger","sun_db","Oracle8","");
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
        String query_text = "SELECT * FROM tab WHERE tname LIKE :tname";
        ua.kiev.grad.CosQuery.RecordDescriptionHolder s1RecordDescriptionHolder = new ua.kiev.grad.CosQuery.RecordDescriptionHolder();
        ua.kiev.grad.CosQuery.FieldDescription fd = new ua.kiev.grad.CosQuery.FieldDescription(":tname", ValueType.TypeString, 30, (short)0, (short)0);
        s1RecordDescriptionHolder.value = new ua.kiev.grad.CosQuery.FieldDescription[1];
        s1RecordDescriptionHolder.value[0] = fd;
        RecordSeqHolder s1RecordSeqHolder = new RecordSeqHolder();
        s1RecordSeqHolder.value = new org.omg.CosQueryCollection.FieldValue[1][1];
        s1RecordSeqHolder.value[0][0] = new org.omg.CosQueryCollection.FieldValue();
        try 
        {
            UAKGQuery query = queryManager.create_query(query_text, 0);
            query.prepare_query(s1RecordDescriptionHolder.value);
            while(true)
            {
                System.out.print("enter tname or 'q' to quit:");
                byte[] arb_tname = new byte[30]; 
                System.in.read(arb_tname);
                if ((char)arb_tname[0] == '\r') continue; 
                if ((char)arb_tname[0] == 'q' | (char)arb_tname[0] == 'Q') break;
                String str_tname = new String(arb_tname);
                str_tname = str_tname.substring(0, str_tname.indexOf("\n") - 1);
                FieldValueAccess.setString(s1RecordSeqHolder.value[0][0], str_tname);
                query.execute_records(s1RecordSeqHolder.value);
                BooleanHolder more = new BooleanHolder(true);
                RecordSeqHolder recordSeqHolder = new RecordSeqHolder();
                recordSeqHolder.value = query.fetch_records(1, more);
                printLib.printRecordSeq(System.out, recordSeqHolder);
            }
            query.destroy();
        }
        catch(IOException ex)
        {
            System.out.println("IOException: " + ex);
        }
        catch(CORBAViewException ex)
        {
            System.out.println("CORBAViewException: " + ex);
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