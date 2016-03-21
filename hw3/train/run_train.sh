!#/bin/bash

source ~/.bashrc

spark-submit  step1_get_tweets_info.py
spark-submit  step2_join_stock_ret_with_tweets.py
spark-submit  step3_extract_feature.py
spark-submit  step4_train_model.py
