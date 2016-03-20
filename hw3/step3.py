import os
import sys
from pyspark import SparkConf, SparkContext
from pyspark.sql import SQLContext
APP_NAME = "My Spark Application"

sentimentdic = {}
def extract_feature(content):
  def work(content, dic):
    col = contetn.split(" ")
    res = [0.0,0.0]
    for w in col:
      if sentimentdic.has_key(w.lower()):
        if sentimentdic[w.lower()] == 1:
          res[0] = res[0] +1
        if sentimentdic[w.lower()] == -1:
          res[1] = res[1] + 1
    res[0] = res[0] / (1.0+len(col))      
    res[1] = res[1] / (1.0+len(col))      
  work(content, sentimentdic)


if __name__=="__main__":
  conf = SparkConf().setAppName(APP_NAME)
  conf = conf.setMaster("local[*]")
  sc = SparkContext(conf=conf)
  retdata = sc.textFile(st_ret);
  twsdata = sc.textFile(tws_info);
 
  sentimentdic_src = sc.textFile(words_dic);
  for p in sentimentdic.collect():
    cols = p.split("\t")
    sentimentdic[cols[0]] = cols[1]

  rdd1=rdd.flatMap(mapp)
  print rdd1.count()
  rdd1.saveAsTextFile("tws4")
  #df1=sc.parallelize(fr.collect()).saveAsTextFile("tweets/2013.txt")
  #df1.write.save("tweets2/2013.txt",format="text")
  #print df1
