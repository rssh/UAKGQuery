PROMPT creating table PARTY_IN 

create table PARTY_IN
(
   code    VARCHAR2(10),
   name    VARCHAR2(60),
   id      NUMBER(11,0),
   del_mark        NUMBER(11,0),
   login           VARCHAR2(15),
   password        VARCHAR2(15),
   fullname        VARCHAR2(100),
   bankname        VARCHAR2(50),
   acc_number      VARCHAR2(50),
   mfo             VARCHAR2(30),
   zkpo            VARCHAR2(30),
   ndsid           VARCHAR2(20),
   taxid           VARCHAR2(20),
   address         VARCHAR2(200),
   phone           VARCHAR2(20),
   agreem          VARCHAR2(20),
   firstday        DATE
);

PROMPT creating table PARTY_OUT 

create table PARTY_OUT
(
code    VARCHAR(10),
name    VARCHAR2(60),
id      NUMBER(11,0),
del_mark        NUMBER(11,0),
login           VARCHAR2(15),
password        VARCHAR2(15),
fullname        VARCHAR2(100),
bankname        VARCHAR2(50),
acc_number      VARCHAR2(50),
mfo             VARCHAR2(30),
zkpo            VARCHAR2(30),
ndsid           VARCHAR2(20),
taxid           VARCHAR2(20),
address         VARCHAR2(200),
phone           VARCHAR2(20),
agreem          VARCHAR2(20),
firstday        DATE
);

