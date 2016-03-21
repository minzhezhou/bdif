from pyspark.mllib.regression import LabeledPoint, LinearRegressionWithSGD, LinearRegressionModel
import os
import random
import sys
from pyspark import SparkConf, SparkContext
from pyspark.sql import SQLContext
APP_NAME = "prediction"
stock_cnt = 9

# Load and parse the data
def parsePoint(ss):
  line = ss.split(" ")
  values = [float(x) for x in line]
  return LabeledPoint(values[0], values[1:])

if __name__=="__main__":
  conf = SparkConf().setAppName(APP_NAME)
  conf = conf.setMaster("local[*]")
  sc = SparkContext(conf=conf)

  model_param = sc.textFile("model_param")
  pred_data = sc.textFile("pred_input")

  # Build the model
  model = LinearRegressionWithSGD.setModel(model_param)

  # Evaluate the model on training data
  pred_res = pred_data.map(lambda fea: model.predict(fea))
  pred_res.saveAsTextFile("pred_res")
