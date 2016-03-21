import os
import sys
from pyspark import SparkConf, SparkContext
from pyspark.sql import SQLContext
APP_NAME = "My Spark Application"
def mapp(x):
  compdic = {'0':"boeing", '1':"google",  '2':"amd", '3':"dupont", '4':"intel",   '5':"pfizer",  '6':"merck",  '7':"nike", '8':"verizon"}
  dic2={'boeing':1,'google':3,'amd':0,'dupont':2,'intel':4,'pfizer':7,'merck':5,'nike':6,'verzion':8}
  context=x[1][0]
  return3=x[1][1]
  context2=context.split("\t")[1]
  key=context.split("\t")[0]
  return2=return3.split(",")[dic2[compdic[key]]]
  res="\t".join([x[0],key,return2,context2])
  return res

if __name__=="__main__":
  conf = SparkConf().setAppName(APP_NAME)
  conf = conf.setMaster("local[*]")
  sc = SparkContext(conf=conf)
  df = sc.textFile("stock_return")
  parts = df.map(lambda x: (x.split(",")[0], ",".join(x.split(",")[1:])))
  df2=sc.textFile("tws4")
  parts2=df2.map(lambda l: ((l.split("\t")[0],"\t".join(l.split("\t")[1:]))))
  parts3=parts2.join(parts)
  part4=parts3.map(mapp)
  part4.saveAsTextFile("step2")

