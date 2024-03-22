.. _roq-bybit:
.. _roq-bybit-v5:

.. |checkmark| unicode:: U+2713

roq-bybit
=========


Links
-----

* `Website <https://www.bybit.com/en-US/>`__
* `Documentation <https://bybit-exchange.github.io/docs/v5/intro>`__


Purpose
-------

* Maintain network connectivity with the Bybit exchange
* Route exchange updates to connected clients
* Route client requests to the relevant exchange accounts
* Stream all messages to an event-log


Overview
--------

.. grid::  2
  :gutter: 2

  .. grid-item-card::  Products

    .. list-table::
      :widths: auto

      * - Spot
        - |checkmark|
      * - Futures
        - |checkmark|
      * - Options
        - |checkmark|

  .. grid-item-card::  Market Data

    .. list-table::
      :widths: auto

      * - Reference Data
        - |checkmark|
      * - Market Status
        - |checkmark|
      * - Top of Book
        - |checkmark|
      * - Market by Price (L2)
        - |checkmark|
      * - Market by Order (L3)
        -
      * - Trade Summary
        - |checkmark|
      * - Statistics
        - |checkmark|

  .. grid-item-card::  Order Management

    .. list-table::
      :widths: auto

      * - Create
        - |checkmark|
      * - Modify
        - |checkmark|
      * - Cancel
        - |checkmark|
      * - Cancel All
        - |checkmark|
      * - Auto Cancellation
        -

  .. grid-item-card::  Account Management

    .. list-table::
      :widths: auto

      * - Positions
        - |checkmark|
      * - Funds
        - |checkmark|

* Data center located in: TBD


Conda
-----

* :ref:`Using Conda <tutorial-conda>`

.. tab:: Install

  .. code-block:: bash

    $ mamba install \
          --channel https://roq-trading.com/conda/stable \
          roq-bybit

.. tab:: Configure

  .. code-block:: bash

    $ cp $CONDA_PREFIX/share/roq-bybit/config.toml $CONFIG_FILE_PATH

    # Then modify $CONFIG_FILE_PATH to match your specific configuration

.. tab:: Run

  .. code-block:: bash

    $ roq-bybit \
          --name "bybit" \
          --api "spot" \
          --config_file "$CONFIG_FILE_PATH" \
          --client_listen_address "$UNIX_SOCKET_PATH" \
          --service_listen_address "$TCP_LISTEN_PORT" \
          --flagfile "$FLAG_FILE"


Config
------

* :ref:`Common Config <gateway-config>`


Flags
-----

* :ref:`Using Flags <abseil-cpp>`
* :ref:`Common Flags <gateway-flags>`

.. code-block:: bash

   $ roq-bybit --help

.. tab:: Flags

   .. include:: flags/flags.rstinc

.. tab:: REST

   .. include:: flags/rest.rstinc

.. tab:: WS

   .. include:: flags/ws.rstinc

.. tab:: Download

   .. include:: flags/download.rstinc

.. tab:: MBP

   .. include:: flags/mbp.rstinc

.. tab:: Request

   .. include:: flags/request.rstinc

.. tab:: Common

   .. include:: flags/common.rstinc


Environments
------------

.. code-block:: bash

  $ $CONDA_PREFIX/share/roq-bybit/flags

.. tab:: Prod

   .. include:: flags/prod/flags.cfg
     :code: ini

.. tab:: Test

   .. include:: flags/test/flags.cfg
     :code: ini


Market Data
-----------

.. tab:: Live

  .. list-table::
    :header-rows: 1
    :widths: auto

    * - Event
      - Stream
      - Messages
      - Comments

    * - :cpp:class:`roq::ReferenceData`
      -
      -
      -

    * - :cpp:class:`roq::MarketStatus`
      -
      -
      -

    * - :cpp:class:`roq::TopOfBook`
      - MarketData
      - * orderbook.1.<symbol> (spot)
        * tickers.<symbols> (other)
      -

    * - :cpp:class:`roq::MarketByPriceUpdate`
      - MarketData
      - orderbook.<ws_mbp_depth>.<symbol>
      -

    * - :cpp:class:`roq::MarketByOrderUpdate`
      -
      -
      -

    * - :cpp:class:`roq::TradeSummary`
      - MarketData
      - publicTrade.<symbol>
      -

    * - :cpp:class:`roq::StatisticsUpdate`
      - MarketData
      - tickers.<symbol>
      -

.. tab:: Download

  .. list-table::
    :header-rows: 1
    :widths: auto

    * - Event
      - Stream
      - Messages
      - Comments

    * - :cpp:class:`roq::ReferenceData`
      - Rest
      - /v5/market/instruments-info
      -

    * - :cpp:class:`roq::MarketStatus`
      - Rest
      - /v5/market/instruments-info
      -

    * - :cpp:class:`roq::TopOfBook`
      -
      -
      -

    * - :cpp:class:`roq::MarketByPriceUpdate`
      -
      -
      -

    * - :cpp:class:`roq::MarketByOrderUpdate`
      -
      -
      -

    * - :cpp:class:`roq::TradeSummary`
      -
      -
      -

    * - :cpp:class:`roq::StatisticsUpdate`
      -
      -
      -


Statistics
~~~~~~~~~~

.. list-table::
  :header-rows: 1
  :widths: auto

  * - Type
    - Comments

  * - :cpp:class:`HIGHEST_TRADED_PRICE`
    - (tickers) :code:`h`

  * - :cpp:class:`LOWEST_TRADED_PRICE`
    - (tickers) :code:`l`

  * - :cpp:class:`CLOSE_PRICE`
    - (tickers) :code:`c`

  * - :cpp:class:`TRADE_VOLUME`
    - (tickers) :code:`v`


Order Management
----------------

.. tab:: Live

  .. list-table::
    :header-rows: 1
    :widths: auto

    * - Event
      - Stream
      - Messages
      - Comments

    * - :cpp:class:`roq::OrderUpdate`
      - DropCopy
      - order
      -

    * - :cpp:class:`roq::TradeUpdate`
      - DropCopy
      - execution
      -

.. tab:: Download

  .. list-table::
    :header-rows: 1
    :widths: auto

    * - Event
      - Stream
      - Messages
      - Comments

    * - :cpp:class:`roq::OrderUpdate`
      - OrderEntry
      - /v5/order/realtime
      -

    * - :cpp:class:`roq::TradeUpdate`
      - OrderEntry
      - /v5/execution/list
      -

.. tab:: Request

  .. list-table::
    :header-rows: 1
    :widths: auto

    * - Event
      - Stream
      - Messages
      - Comments

    * - :cpp:class:`roq::CreateOrder`
      - OrderEntry
      - /v5/order/create
      -

    * - :cpp:class:`roq::ModifyOrder`
      - OrderEntry
      - /v5/order/amend
      -

    * - :cpp:class:`roq::CancelOrder`
      - OrderEntry
      - /v5/order/cancel
      -

    * - :cpp:class:`roq::CancelAllOrders`
      - OrderEntry
      - /v5/order/cancel-all
      -

.. tab:: Response

  .. list-table::
    :header-rows: 1
    :widths: auto

    * - Event
      - Stream
      - Messages
      - Comments

    * - :cpp:class:`roq::OrderAck`
      - OrderEntry
      -
      -

Order Types
~~~~~~~~~~~

.. list-table::
  :header-rows: 1
  :widths: auto

  * - Type
    - Comments

  * - :cpp:class:`MARKET`
    - Mapped to :code:`'MARKET'` (JSON)

  * - :cpp:class:`LIMIT`
    - Mapped to :code:`'LIMIT'` or :code:`'LIMIT_MAKER'` (JSON)


Time in Force
~~~~~~~~~~~~~

.. list-table::
  :header-rows: 1
  :widths: auto

  * - Type
    - Comments

  * - :cpp:class:`GTC`
    - Mapped to :code:`'GTC'` (JSON)

  * - :cpp:class:`FOK`
    - Mapped to :code:`'FOK'` (JSON)

  * - :cpp:class:`IOC`
    - Mapped to :code:`'IOC'` (JSON)


Position Effect
~~~~~~~~~~~~~~~

.. note::

  Not supported

Execution Instructions
~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
  :header-rows: 1
  :widths: auto

  * - Type
    - Comments

  * - :cpp:class:`PARTICIPATE_DO_NOT_INITIATE`
    - Mapped to :code:`'LIMIT_MAKER'` (JSON)


Account Management
------------------

.. tab:: Live

  .. list-table::
    :header-rows: 1
    :widths: auto

    * - Event
      - Stream
      - Messages
      - Comments

    * - :cpp:class:`roq::PositionUpdate`
      - DropCopy
      - position
      -

    * - :cpp:class:`roq::FundsUpdate`
      - DropCopy
      - wallet
      -

.. tab:: Download

  .. list-table::
    :header-rows: 1
    :widths: auto

    * - Event
      - Stream
      - Messages
      - Comments

    * - :cpp:class:`roq::PositionUpdate`
      - OrderEntry
      - /v5/position/list"
      -

    * - :cpp:class:`roq::FundsUpdate`
      - OrderEntry
      - /v5/account/wallet-balance
      -


Streams
-------

.. tab:: Rest

  .. list-table::
    :header-rows: 1
    :widths: auto

    * - Type
      - Comments

    * - REST
      - Primary purpose

        * discover the full list of symbols

.. tab:: MarketData

  .. list-table::
    :header-rows: 1
    :widths: auto

    * - Type
      - Comments

    * - WebSocket
      - Primary purpose

        * live market data

        Each connection

        * supports a slice of the symbols

.. tab:: OrderEntry

  .. list-table::
    :header-rows: 1
    :widths: auto

    * - Type
      - Comments

    * - REST
      - Primary purpose

        * Order actions

.. tab:: DropCopy

  .. list-table::
    :header-rows: 1
    :widths: auto

    * - Type
      - Comments

    * - WebSocket
      - Primary purpose

        * order updates


Constraints
-----------

* The gateway can not simultaneously support all product categories due to
  overlapping symbol names, e.g. BTCUSDT being both spot and linear.
  For this reason, the :code:`--api` flag controls the product category and, if
  necessary, the :code:`--name` or :code:`--exchange` flags must be configured
  to appropriately differentitate the sources.

* The :code:`order` channel doesn't give us any information about last traded,
  only the aggregate fields (traded / remaining / average price) are available.
  The last trade price/quantity fields are therefore estimated.

  .. note::
     The :code:`execution` channel will independently report the fills.

Comments
--------

* :code:`TopOfBook` is based on :code:`orderbook.1` for spot and :code:`tickers`
  for all other categories.
