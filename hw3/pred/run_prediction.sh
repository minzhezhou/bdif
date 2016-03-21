!#/bin/bash

source ~/.bashrc

spark-submit step1_gen_pred_input.py 
spark-submit step2_prediction.py 
