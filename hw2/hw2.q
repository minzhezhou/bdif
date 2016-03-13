\l config.q

/ generate a random variable from a uniform distribution
uniformRv : {[] 
    (1 ? 1f)[0]
    }

/ generate a list of sample from a multinomial distribution
mulSample : {[lst;n]
    acum:(+\) lst;
    1 + acum bin n?(last acum) }

/ generate a list of sample from a powerlaw distribution    
powerlawSample : { [alpha;beta;n]
    f: {[a;x] 1%(1 + a xexp x)};
    mul:(f[alpha]':)  til "i"$(beta%(alpha-1));
    mulSample[mul;n]
    }

/ generate a random variable from a exponentia distribution    
exponentialRv : { [lambda] 
    neg[log[uniformRv[]]] % lambda 
    }

/ generate a random variable from a standard gaussian    
nextGaussianRvAvailable : 0;
nextGaussianRv : 0f;
gaussianRv : { []
    compute:1;
    if[nextGaussianRvAvailable;
        compute:0;
        result:nextGaussianRv;
        nextGaussianRvAvailable::0;
    ];
    if[compute;
        w:1f;
        while[w>=1f;
            x1:((2f*uniformRv[])-1f);
            x2:((2f*uniformRv[])-1f);
            w:((x1*x1)+(x2*x2))
        ];
        w:sqrt((-2f*log[w])%w);
        y1:x1*w;
        y2:x2*w;

        nextGaussianRv::y2;
        nextGaussianRvAvailable::1;
        result:y1;
    ];
    result
    }

differences : { [lst]
    lst[0] -': lst[(til ((count lst) - 1)) + 1] }

/ calculate gbm factor    
geometricBrownianMotionFactor : { [drift; volatility; dateDifference]
    deterministicTerm : (drift - ((0.5 * (volatility xexp 2.0)))) * dateDifference;
    stochasticTerm : volatility * gaussianRv[] * sqrt[dateDifference];
    exp[deterministicTerm + stochasticTerm] }

/ generate daily gbm process    
geometricBrownianMotion : { [initialValue; drift; volatility; dateTimes]
    drift %: 365.25;
    volatility %: sqrt[365.25];
    dateDifferences : differences[dateTimes];

    f : geometricBrownianMotionFactor[drift; volatility];
    factors : f':[dateDifferences];

    (initialValue), initialValue *\ factors }

/ generate timestap follow poission process
exponentiallyDistributedDateTimes : { [ticksPerDay; beginDateTime; endDateTime]
    dateTimes : `datetime$();
    lastDateTime : beginDateTime + exponentialRv[ticksPerDay];

    while[lastDateTime <= endDateTime;
        dateTimes ,: lastDateTime;
        lastDateTime : lastDateTime + exponentialRv[ticksPerDay];
    ];
    dateTimes }

/ generate price series and timestamp togather    
geometricBrownianMotionTimeSeries : { [initialValue; drift; volatility; ticksPerDay; beginDateTime; endDateTime]

    dateTimes : exponentiallyDistributedDateTimes[ticksPerDay; beginDateTime; endDateTime];
    dates : `date$dateTimes;
    times : `time$dateTimes;              
    dataPoints : geometricBrownianMotion[initialValue; drift; volatility; dateTimes];
    /`DATE`TIME`price!(dates; times; dataPoints) }
    `TIME`price!(dateTimes; dataPoints) }

check_file_exists: {[file_]
  not () ~ key hsym "S"$ file_
  };
    
load_symbol_file: {[file_]

  if [not check_file_exists[file_];
    0N!(string .z.Z), "file not found",;
    :()
  ];

  / the file must be formatted like:
  /  SYMBOL,drift,sigma,intensity, start_date, price
  /  AA,0.5,0.3,20000,2014.1.1T00:00:00,100
  /  GS,0.6,0.6,10000,2014.1.1T09:00:00,50
  /  ..
  `stocks set
   ("SFFIZF"; enlist ",") 0: hsym "S"$ file_;
   0N!(string .z.Z), "symbol loaded there are ", (string count stocks), " stocks",; }

gen_stock_trades: {[stock;days]
    stock_info: (value stock); 
    sym:stock_info 0;
    drift: stock_info 1;
    sigma: stock_info 2;
    intensity: stock_info 3;
    st:stock_info 4;
    price: stock_info 5;
    end:st + days;
    
    ts:geometricBrownianMotionTimeSeries [price;drift; sigma; intensity; st; end];
    vol:powerlawSample[powerlaw_alpha;powerlaw_beta;count ts((key ts)0)];
    ts[`volume]:vol + (count vol) ? 10;
    flip ts }

gen_time_grid: {[start;end;delta]
    a:`datetime$start;
    dcnt: `int$((`datetime$end - a)*24*60%delta);
    dt: (a + (delta%(24*60)) * (til dcnt));
    `grid set 
    flip (enlist `TIME) ! (enlist dt)
               }

ema: {{z+x*y} \[ first y; (1-x); x*y ] }

save_csv: {[file_; table_]
    (hsym "S"$ file_) 0: .h.cd table_; }      

calc_vwap: { [cur_st;days]
    st_name:string cur_st[`SYMBOL];
    trades:gen_stock_trades [cur_st; days];
    save_csv[script_path,st_name,".in.csv";trades];
    tbegin:exec first TIME from trades;
    tend:exec last TIME from trades;
    gen_time_grid [tbegin;tend;bar_interval];
    t: ((cols trades), `CNT) xcols grid ,' (update CNT:i from trades) asof grid;
    trade_bar:update volbar: sum each (0,1_t[`CNT]) _ trades[`volume], CNT: deltas CNT from t;
    /update ema_volbar:(first volbar) + ema[0.9;volbar - (first volbar)] from `trade_bar;
    ema_volbar:ema[0.95;trade_bar[`volbar]];
    ema_pri:ema[0.95;trade_bar[`price]];
    price_star:ema[0.95;trade_bar[`price]*trade_bar[`volbar]];
    trade_bar: update ema_price:ema_pri, VWAP:price_star%ema_volbar from trade_bar;
    save_csv[script_path,st_name,".out.csv";trade_bar];
    }

load_symbol_file [script_path,"symbol"];
(calc_vwap[;trading_days]':) stocks
