import java.io.*;

import org.omg.CORBA.*;
import org.omg.CosQuery.*;
import org.omg.CORBA.ORBPackage.*;
import org.omg.CosQueryCollection.*;

import ua.kiev.grad.RC.*;
import ua.kiev.grad.CosQuery.*;
import ua.kiev.grad.CosQuery.RecordDescriptionHolder;
import ua.kiev.grad.CosQuery.RecordSeqHolder;
import ua.kiev.grad.CosQuery.FieldDescription;

public class printLib
{
    public static ORB orb;
    public static void printRecordSeq(PrintStream out, RecordSeqHolder recordSeq)
    {
        for(int row = 0; row < recordSeq.value.length; row++) 
            printRecord(out, recordSeq.value[row]);
    }
    public static void printRecord(PrintStream out, FieldValue[] record)
    {
        out.print('|');
        for(int col = 0; col < record.length; col++) {
            printField(out, record[col]);
        out.print('|');
        }
        out.print('\n');
    }
    public static void printField(PrintStream out, FieldValue fv)
    {
        if (fv.discriminator()) 
        {
            out.print("NULL");
            return;
        }
        Value v = fv.v();
        switch(v.discriminator().value()) {
            case ValueType._TypeBoolean:
                out.print((v.b()?'T':'F')); 
                break;
            case ValueType._TypeChar:
                out.print(v.c());
                break;
            case ValueType._TypeOctet:
                out.print(v.o());
                break;
            case ValueType._TypeShort:
                out.print(v.s());
                break;
            case ValueType._TypeUShort:
                out.print(v.us());
                break;
            case ValueType._TypeLong:
                out.print(v.l());
                break;
            case ValueType._TypeULong:
                out.print(v.ul());
                break;
            case ValueType._TypeFloat:
                out.print(v.f());
                break;
            case ValueType._TypeDouble:
                out.print(v.d());
                break;
            case ValueType._TypeString:
                out.print(v.str());
                break;
            case ValueType._TypeObject:
                {
                    String s = orb.object_to_string(v.obj());
                    out.print(s);
                }
                break;
            case ValueType._TypeAny:
                out.print("Any ??");
                break;
            case ValueType._TypeSmallInt:
                out.print(v.si());
                break;
            case ValueType._TypeInteger:
                out.print(v.i());
                break;
            case ValueType._TypeReal:
                out.print(v.r());
                break;
            case ValueType._TypeDoublePrecision:
                out.print(v.dp());
                break;
            case ValueType._TypeCharacter:
                out.print(v.ch());
                break;
            case ValueType._TypeDecimal:
                printDecimal(out,v.dec());
                break;
            case ValueType._TypeNumeric:
                printDecimal(out,v.n());
                break;
            case ValueType._TypeRaw:
                byte[] raw = v.raw();
                printRaw(out,raw);
                break;
            case ValueType._TypeLongRaw:
                byte[] lrawid = v.lrawid();
                printRaw(out,lrawid);
                break;
            case ValueType._TypeLongString:
                {
                    String s = v.lstr();
                    out.print(s);
                }
                break;
            /*case ValueType._TypeWString:
                {
                    String s = v.wstr();
                    out.print(s);
                }
                break;*/
            default:
                out.print("UnknownFieldType");
        }
    }
    public static void printDecimal(PrintStream out, Decimal dec)
    {
        out.print("prec: " + dec.precision + ", scale:" + dec.scale + ", value:");
        printRaw(out, dec.value);
    }

    public static void printRaw(PrintStream out, byte[] octSeq)
    {
        for(int i = 0; i < octSeq.length; i++)
        {
            int x = (int)(octSeq[i]);
            out.print('[' + x + ']');
        }
    }
    public static void printRC(PrintStream out, byte[] octSeq) 
        throws BadOctSeq
    {
        RCHeader header = new RCHeader((byte)0,0,0);
        RCHeaderHolder headerHolder = new RCHeaderHolder(header);
        IntHolder pos = new IntHolder(0);
        ReaderHolder reader = new ReaderHolder();
        RCReader.readHeader(header, pos, octSeq);
        out.print("Header: nFields = " + header.nFields + '\n');
        out.print("Header: nRecords = " + header.nRecords + '\n');
        for(int nRecord = 0; nRecord < header.nRecords; nRecord++)
        {
            for(int nField = 0; nField < header.nFields; nField++)
            {
                FieldValue fv = RCReader.readField(pos, octSeq);
                printField(out, fv);
                out.print('|');
            }
            out.print('\n');
        }
    }
}