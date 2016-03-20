import os
import sys
from pyspark import SparkConf, SparkContext
from pyspark.sql import SQLContext
APP_NAME = "My Spark Application"

def get_compid(strs):
  compdic = {'boeing':"0", 'google':"1",  'amd':"2",  'dupont':"3", 'intel':"4",   'pfizer':"5",  'merck':"6",  'nike':"7",  'verizon':"8"}

  res = [];
  scol = strs.split(" ")
  for s in scol:
    if compdic.has_key(s.lower()):
      res.append(compdic[s.lower()]);
    return res;

def str_date(x):
  month={"Jan":"01","Feb":"02","Mar":"03"}
  x1=x.split(" ")
  y=x1[5]
  m=month[x1[1]]
  d=x1[2]
  return "-".join([y,m,d])

def mapp(x):
  tmstp = str_date(x.created_at)
  compid = get_compid(x.text)
  res = []
  for id in compid:
    res.append(tmstp + "\t" + id + "\t" + x.text)
  return res

if __name__=="__main__":
  conf = SparkConf().setAppName(APP_NAME)
  conf = conf.setMaster("local[*]")
  sc = SparkContext(conf=conf)
  sqlContext = SQLContext(sc)
  df = sqlContext.read.json("tws1")
  df = df.filter(df["lang"].like('%en%'))
  df=df.select(["created_at","text"])
  rdd=df.rdd
  rdd1=rdd.flatMap(mapp)
  print rdd1.count()
  rdd1.saveAsTextFile("tws4")
  #df1=sc.parallelize(fr.collect()).saveAsTextFile("tweets/2013.txt")
  #df1.write.save("tweets2/2013.txt",format="text")
  #print df1
