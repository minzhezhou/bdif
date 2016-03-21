import os
import sys
from pyspark import SparkConf, SparkContext
from pyspark.sql import SQLContext

APP_NAME = "Join_stock_return_tweet_info"

def trans_ret_pair(ln):
  cols = ln.strip().split(",")
  t = cols[0]
  res = []
  for i in xrange(1, len(cols)):
    res.append(t + "\t" + str(i-1) + "\t" + cols[i])
  return res  

def pair_ret_tws(x):
  tm = x[0][0]    
  stid = x[0][1]
  context=x[1][0]
  ret=x[1][1]
  res="\t".join([tm,stid,ret,context])
  return res

if __name__=="__main__":
  conf = SparkConf().setAppName(APP_NAME)
  conf = conf.setMaster("local[*]")
  sc = SparkContext(conf=conf)
  df = sc.textFile("stock_return")
  stret = df.flatMap(trans_ret_pair).map(lambda x: ((x.split("\t")[0], x.split("\t")[1]), (x.split("\t")[2])))
  df2=sc.textFile("step1")
  tws_info=df2.map(lambda l: (((l.split("\t")[0], l.split("\t")[1]),  (l.split("\t")[2]))))
  tws_ret=tws_info.join(stret)
  res=tws_ret.map(pair_ret_tws)
  res.saveAsTextFile("step2")

