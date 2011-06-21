package ua.kiev.grad.RC;

/*
 * Coder
 * (C) Copyright 2000 Alexander Bourla <AlexB@grad.kiev.ua>
 * $Source: /home/rssh/cvs-dump/cvsroot/COS/CosQuery.1/jsrc/ua/kiev/grad/RC/RCWriter.java,v $
 * $Id: RCWriter.java,v 1.3 2000-03-03 11:25:28 alexb Exp $
 * Last modified by: $Author: alexb $
 */

import org.omg.CosQueryCollection.*;

public class RCWriter // implements Writer
{
    private static void
    enlarge(int nOctets, 
            org.omg.CORBA.IntHolder pos, 
            OctSeqHolder octSeq)
    {
        int n = octSeq.value.length; 
        int req_n = pos.value + nOctets; 
        boolean changeFlag = false;
        while (req_n > n) 
        {
            if (n == 0) n = req_n;
            else if (n < 4000) n *= 2; 
            else n += nOctets;
            changeFlag = true;
        }
        if (changeFlag)
        {
            byte[] tempOctSeq = new byte[n];
            System.arraycopy(octSeq.value, 0, tempOctSeq, 0, octSeq.value.length);
            octSeq.value = tempOctSeq;
        }
    }
    
    public static void
    writeHeader(int nRecords,
                int nFields,
                org.omg.CORBA.IntHolder pos,
                OctSeqHolder octSeq)
        throws InvalidHeadData
    {
        pos.value = 0;
        writeHead(pos, octSeq);
        writeNumberOfRecords(nRecords, pos, octSeq);
        writeNumberOfFields(nFields, pos, octSeq);
    }

    private static void 
    writeNumberOfRecords(int nRecords,
                         org.omg.CORBA.IntHolder pos, 
                         OctSeqHolder octSeq)
    {
        pos.value = 1;
        writeLong(nRecords, pos, octSeq);
    }

    /*private static void 
    writeNumberOfRecords(int nRecords, 
                         OctSeqHolder octSeq)
    {
        org.omg.CORBA.IntHolder pos = new org.omg.CORBA.IntHolder();
        pos.value = 1;
        writeLong(nRecords, pos, octSeq);
    }*/

    private static void 
    writeNumberOfFields(int nFields,
                        org.omg.CORBA.IntHolder pos, 
                        OctSeqHolder octSeq)
        throws InvalidHeadData
    {
        pos.value = 5;
        if (nFields > 255) throw new InvalidHeadData();
        writeOctet((byte)nFields, pos, octSeq);
    }

    /*private static void 
    writeNumberOfFields(int nFields,
                        OctSeqHolder octSeq)
    {
        org.omg.CORBA.IntHolder pos = new org.omg.CORBA.IntHolder(5);
        writeNumberOfFields(nFields, pos, octSeq);
    }*/
    
    private static void 
    writeOctet(byte value,
               org.omg.CORBA.IntHolder pos,
               OctSeqHolder octSeq)
    {
        enlarge(1, pos, octSeq);
        octSeq.value[pos.value++] = value;
    }
    
    public static void
    writeHead(org.omg.CORBA.IntHolder pos,
              OctSeqHolder octSeq)
    {
        pos.value = 0;
        writeOctet((byte)1, pos, octSeq);
    }

    public static void
    writeRecord(org.omg.CosQueryCollection.FieldValue[] record,
                org.omg.CORBA.IntHolder pos,
                OctSeqHolder octSeq)
        throws TypeNotImplemented
    {
        int n = record.length;
        //writeRecordHeader(n, pos, octSeq);
        if (n == 0) return;
        for(int i = 0; i < n; i++)
            writeField(record[i], pos, octSeq);
    }

    public static void
    writeField(org.omg.CosQueryCollection.FieldValue value,
               org.omg.CORBA.IntHolder pos,
               OctSeqHolder octSeq)
        throws TypeNotImplemented
    {
        if (value.discriminator()) 
        {
            writeNullField(pos, octSeq);
            return;
        }
        org.omg.CosQueryCollection.Value v = value.v();
        switch(v.discriminator().value()) 
        {
            case ValueType._TypeBoolean:  
                writeBooleanField(v.b(), pos, octSeq);
                break;
            case ValueType._TypeChar:
                writeCharField(v.c(), pos, octSeq);
                break;
            case ValueType._TypeOctet:
                writeOctetField(v.o(), pos, octSeq);
                break;
            case ValueType._TypeShort:
                writeShortField(v.s(), pos, octSeq);
                break;
            case ValueType._TypeUShort:
                writeUShortField(v.us(), pos, octSeq);
                break;
            case ValueType._TypeLong:
                writeLongField(v.l(), pos, octSeq);
                break;
            case ValueType._TypeULong:
                writeULongField(v.l(), pos, octSeq);
                break;
            case ValueType._TypeFloat:
                writeFloatField(v.f(), pos, octSeq);
                break;
            case ValueType._TypeDouble:
                writeDoubleField(v.d(), pos, octSeq);
                break;
            case ValueType._TypeString:
                writeStringField(v.str(), pos, octSeq);
                break;
            case ValueType._TypeObject:
                writeObjectField(v.obj(), pos, octSeq);
                break;
            case ValueType._TypeAny:
                writeAnyField(v.a(), pos, octSeq);
                break;
            case ValueType._TypeSmallInt:
                writeShortField(v.si(), pos, octSeq);
                break;
            case ValueType._TypeInteger:
                writeLongField(v.i(), pos, octSeq);
                break;
            case ValueType._TypeDecimal:
                writeDecimalField(v.dec(), pos, octSeq);
                break;
            case ValueType._TypeNumeric:
                writeDecimalField(v.n(), pos, octSeq);
                break;
            case ValueType._TypeRaw:
                writeRawField(v.raw(), pos, octSeq);
                break;
            case ValueType._TypeLongRaw:
                writeLongRawField(/*v.long_raw()*/v.lrawid(), pos, octSeq);
                break;
            case ValueType._TypeLongString:
                writeLongStringField(/*v.long_string()*/v.lstr(), pos, octSeq);
                break;
            default:
                throw new TypeNotImplemented();
        }
    }
    
    public static void
    writeRecordSeq(org.omg.CosQueryCollection.FieldValue[][] recordSeq,
                   org.omg.CORBA.IntHolder pos,
                   OctSeqHolder octSeq)
        throws TypeNotImplemented
    {
        int n = recordSeq.length;
        for(int i = 0; i < n; i++) 
            writeRecord(recordSeq[i], pos, octSeq);
    }

    public static void
    writeBoolean(boolean value,
                 org.omg.CORBA.IntHolder pos,
                 OctSeqHolder octSeq)
    {
        writeOctet((byte)(value ? 1 : 0),pos,octSeq);
    }

    public static void
    writeChar(char value,
              org.omg.CORBA.IntHolder pos,
              OctSeqHolder octSeq)
    {
        enlarge(1, pos, octSeq);
        octSeq.value[pos.value++] = (byte)value;
    }

    public static void
    writeShort(short value,
               org.omg.CORBA.IntHolder pos,
               OctSeqHolder octSeq)
    {
        enlarge(2,pos,octSeq);
        octSeq.value[pos.value++] = (byte)(value & 0xff);
        octSeq.value[pos.value++] = (byte)(value >> 8);
    }

    public static void
    writeLong(int value,
              org.omg.CORBA.IntHolder pos,
              OctSeqHolder octSeq)
    {
        writeULong(value, pos, octSeq);
    }

    public static void
    writeULong(int value,
               org.omg.CORBA.IntHolder pos,
               OctSeqHolder octSeq)
    {
        enlarge(4, pos, octSeq);
        for (byte i = 0; i < 4; i++)
            octSeq.value[pos.value++] = (byte)((value >> 8*i) & 0xff);
    }

    public static void
    writeFloat(float value,
               org.omg.CORBA.IntHolder pos,
               OctSeqHolder octSeq)
    {
        writeULong(Float.floatToIntBits(value), pos, octSeq); 
    }

    public static void
    writeDouble(double value,
                org.omg.CORBA.IntHolder pos,
                OctSeqHolder octSeq)
    {
        writeULong((int)Double.doubleToLongBits(value), pos, octSeq); 
    }

    public static void
    writeString(String value,
                org.omg.CORBA.IntHolder pos,
                OctSeqHolder octSeq)
    {
        int n = value.length();
        enlarge(n + 4, pos, octSeq);
        writeULong(n, pos, octSeq);
        System.arraycopy(value.getBytes(), 0, octSeq.value, pos.value, n);
        pos.value += n;
    }

    public static void
    writeObject(org.omg.CORBA.Object value,
                org.omg.CORBA.IntHolder pos,
                OctSeqHolder octSeq)
        throws TypeNotImplemented
    {
        throw new TypeNotImplemented(ValueType.TypeObject);
    }

    public static void
    writeAny(org.omg.CORBA.Any value,
             org.omg.CORBA.IntHolder pos,
             OctSeqHolder octSeq)
        throws TypeNotImplemented
    {
        throw new TypeNotImplemented(ValueType.TypeAny);
    }

    public static void
    writeDecimal(org.omg.CosQueryCollection.Decimal value,
                 org.omg.CORBA.IntHolder pos,
                 OctSeqHolder octSeq)
    {
        writeLong(value.precision, pos, octSeq);
        writeLong(value.scale, pos, octSeq);
        writeRaw(value.value, pos, octSeq);
    }

    public static void
    writeRaw(byte[] value,
             org.omg.CORBA.IntHolder pos,
             OctSeqHolder octSeq)
    {
        int n = value.length;
        enlarge(n + 4, pos, octSeq);
        writeULong(n, pos, octSeq); 
        System.arraycopy(value, 0, octSeq.value, pos.value, n);
        pos.value += n;

    }

    /*public static void
    writeFieldValue(org.omg.CosQueryCollection.FieldValue value,
                    org.omg.CORBA.IntHolder pos,
                    OctSeqHolder octSeq)
    { Need not be in IDL
    }*/

    public static void
    writeNullField(org.omg.CORBA.IntHolder pos,
                   OctSeqHolder octSeq)
    {
        writeOctet((byte)0x00, pos, octSeq);
    }

    public static void
    writeBooleanField(boolean value,
                      org.omg.CORBA.IntHolder pos,
                      OctSeqHolder octSeq)
    {
        writeOctet((byte)0x01, pos, octSeq);
        writeBoolean(value, pos, octSeq);
    }

    public static void
    writeCharField(char value,
                   org.omg.CORBA.IntHolder pos,
                   OctSeqHolder octSeq)
    {
        writeOctet((byte)0x02, pos, octSeq);
        writeChar(value, pos, octSeq);
    }

    public static void
    writeOctetField(byte value,
                    org.omg.CORBA.IntHolder pos,
                    OctSeqHolder octSeq)
    {
        writeOctet((byte)0x03, pos, octSeq);
        writeOctet(value, pos, octSeq);
    }

    public static void
    writeShortField(short value,
                    org.omg.CORBA.IntHolder pos,
                    OctSeqHolder octSeq)
    {
        writeOctet((byte)0x04, pos, octSeq);
        writeShort(value, pos, octSeq);
    }

    public static void
    writeUShortField(short value,
                     org.omg.CORBA.IntHolder pos,
                     OctSeqHolder octSeq)
    {
        writeOctet((byte)0x05, pos, octSeq);
        writeShort(value, pos, octSeq);
    }

    public static void
    writeLongField(int value,
                   org.omg.CORBA.IntHolder pos,
                   OctSeqHolder octSeq)
    {
        writeOctet((byte)0x06, pos, octSeq); 
        writeLong(value, pos, octSeq); 
    }

    public static void
    writeULongField(int value,
                    org.omg.CORBA.IntHolder pos,
                    OctSeqHolder octSeq)
    {
        writeOctet((byte)0x07, pos, octSeq);
        writeULong(value, pos, octSeq);
    }

    public static void
    writeFloatField(float value,
                    org.omg.CORBA.IntHolder pos,
                    OctSeqHolder octSeq)
    {
        writeOctet((byte)0x08, pos, octSeq);
        writeFloat(value, pos, octSeq);
    }

    public static void
    writeDoubleField(double value,
                     org.omg.CORBA.IntHolder pos,
                     OctSeqHolder octSeq)
    {
        writeOctet((byte)0x09, pos, octSeq);
        writeDouble(value, pos, octSeq);
    }

    public static void
    writeStringField(String value,
                     org.omg.CORBA.IntHolder pos,
                     OctSeqHolder octSeq)
    {
        writeOctet((byte)0x0A, pos, octSeq);
        writeString(value, pos, octSeq);
    }

    public static void
    writeObjectField(org.omg.CORBA.Object value,
                     org.omg.CORBA.IntHolder pos,
                     OctSeqHolder octSeq)
        throws TypeNotImplemented
    {
        writeOctet((byte)0x0B, pos, octSeq);
        writeObject(value, pos, octSeq);
    }

    public static void
    writeAnyField(org.omg.CORBA.Any value,
                  org.omg.CORBA.IntHolder pos,
                  OctSeqHolder octSeq)
        throws TypeNotImplemented
    {
        writeOctet((byte)0x0C, pos, octSeq);
        writeAny(value, pos, octSeq);
    }

    public static void
    writeDecimalField(org.omg.CosQueryCollection.Decimal value,
                      org.omg.CORBA.IntHolder pos,
                      OctSeqHolder octSeq)
    {
        writeOctet((byte)0x0F, pos, octSeq);
        writeDecimal(value, pos, octSeq);
    }

    public static void
    writeRawField(byte[] value,
                  org.omg.CORBA.IntHolder pos,
                  OctSeqHolder octSeq)
    {
        writeOctet((byte)0x11, pos, octSeq);
        writeRaw(value, pos, octSeq);
    }

    public static void
    writeLongRawField(byte[] value,
                      org.omg.CORBA.IntHolder pos,
                      OctSeqHolder octSeq)
    {
        writeOctet((byte)0x12, pos, octSeq);
        writeRaw(value, pos, octSeq);
    }

    public static byte[]
    copyStream(int from_pos,
               int to_pos,
               byte[] octSeq)
        throws InvalidPosition
    {
        if(to_pos > from_pos) throw new InvalidPosition();
        int n = from_pos - to_pos + 1;
        byte[] retval = new byte[n];
        System.arraycopy(octSeq, from_pos, retval, 0, n);
        return retval;
    }
    
    private static void 
    writeLongStringField(/*byte[]*/String value,
                      org.omg.CORBA.IntHolder pos,
                      OctSeqHolder octSeq)
    {
        writeOctet((byte)0x13, pos, octSeq);
        writeString(value, pos, octSeq);
    }
}
