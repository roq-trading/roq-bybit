# ReferenceData

## SPOT

```json
{
  symbol="BTCUSDT",
  options_type=<UNDEFINED>,
  contract_type=<UNDEFINED>,
  base_coin="BTC",
  quote_coin="USDT",
  launch_time=0ms,
  delivery_time=0ms,
  delivery_fee_rate=nan,
  price_scale=nan,
  innovation="0",
  status=TRADING,
  leverage_filter={
    min_leverage=nan,
    max_leverage=nan,
    leverage_step=nan
  },
  lot_size_filter={
    base_precision=1.0000000000000004e-06,
    quote_precision=1.0000000000000005e-08,
    min_order_qty=4.800000000000002e-05,
    max_order_qty=71.73956243,
    min_order_amt=1,
    max_order_amt=4000000,
    qty_step=nan,
    post_only_max_order_qty=nan,
    max_mkt_order_qty=nan,
    min_notional_value=nan
  },
  price_filter={
    min_price=nan,
    max_price=nan,
    tick_size=0.010000000000000002
  },
  unified_margin_trade=false,
  funding_interval=0,
  settle_coin="",
  margin_trading="utaOnly",
  copy_trading="",
  upper_funding_rate="",
  lower_funding_rate="",
  is_pre_listing=false
}
```

### LINEAR

* multiplier == 1 ???
* unit = 0.001 BTC ???

```json
{
symbol="BTCUSDT",
options_type=<UNDEFINED>,
contract_type=LINEAR_PERPETUAL,
base_coin="BTC",
quote_coin="USDT",
launch_time=1584230400000ms,
delivery_time=0ms,
delivery_fee_rate=nan,
price_scale=2,
innovation="",
status=TRADING,
leverage_filter={
  min_leverage=1,
  max_leverage=100,
  leverage_step=0.010000000000000002
},
lot_size_filter={
  base_precision=nan,
  quote_precision=nan,
  min_order_qty=0.0010000000000000002,
  max_order_qty=1190,
  min_order_amt=nan,
  max_order_amt=nan,
  qty_step=0.0010000000000000002,
  post_only_max_order_qty=1190,
  max_mkt_order_qty=119,
  min_notional_value=5
},
price_filter={
  min_price=0.1,
  max_price=199999.8,
  tick_size=0.1
},
unified_margin_trade=true,
funding_interval=480,
settle_coin="USDT",
margin_trading="",
copy_trading="both",
upper_funding_rate="0.00375",
lower_funding_rate="-0.00375",
is_pre_listing=false
}
```

## INVERSE

* multiplier == 1 ???
* unit = 1 USD ???

```json
{
  symbol="BTCUSD",
  options_type=<UNDEFINED>,
  contract_type=INVERSE_PERPETUAL,
  base_coin="BTC",
  quote_coin="USD",
  launch_time=1542211200000ms,
  delivery_time=0ms,
  delivery_fee_rate=nan,
  price_scale=2,
  innovation="",
  status=TRADING,
  leverage_filter={
    min_leverage=1,
    max_leverage=100,
    leverage_step=0.010000000000000002
  },
  lot_size_filter={
    base_precision=nan,
    quote_precision=nan,
    min_order_qty=1,
    max_order_qty=25000000,
    min_order_amt=nan,
    max_order_amt=nan,
    qty_step=1,
    post_only_max_order_qty=25000000,
    max_mkt_order_qty=5000000,
    min_notional_value=nan
  },
  price_filter={
    min_price=0.5,
    max_price=999999,
    tick_size=0.5
  },
  unified_margin_trade=true,
  funding_interval=480,
  settle_coin="BTC",
  margin_trading="",
  copy_trading="none",
  upper_funding_rate="0.00375",
  lower_funding_rate="-0.00375",
  is_pre_listing=false
}
```

## OPTION

* multiplier == 1 ???
* strike currency == settle currency ???
* expiry == delivery ???

```json
  {
  symbol="BTC-26SEP25-300000-P",
  options_type=PUT,
  contract_type=<UNDEFINED>,
  base_coin="BTC",
  quote_coin="USDC",
  launch_time=1727942400000ms,
  delivery_time=1758873600000ms, ***
  delivery_fee_rate=0.00015000000000000007,
  price_scale=nan,
  innovation="",
  status=TRADING,
  leverage_filter={
    min_leverage=nan,
    max_leverage=nan,
    leverage_step=nan
  },
  lot_size_filter={
    base_precision=nan,
    quote_precision=nan,
    min_order_qty=0.010000000000000002,
    max_order_qty=500,
    min_order_amt=nan,
    max_order_amt=nan,
    qty_step=0.010000000000000002,
    post_only_max_order_qty=nan,
    max_mkt_order_qty=nan,
    min_notional_value=nan
  },
  price_filter={
    min_price=5,
    max_price=10000000,
    tick_size=5
  },
  unified_margin_trade=false,
  funding_interval=0,
  settle_coin="USDC",
  margin_trading="",
  copy_trading="",
  upper_funding_rate="",
  lower_funding_rate="",
  is_pre_listing=false
  }
```
