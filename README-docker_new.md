### Dockerfile and Docker-compose запуск node
##### Сотав дистрибутива
-  Dockerfile - описание инфраструктуры node. 
>  Описание  источников зависимосей 
>  Компиляция проекта 

-  docker-compose - управление  режимом запуска и поддержка env

 Environmental Variables
   * $CROWDWIZD_SEED_NODES
   * $CROWDWIZD_RPC_ENDPOINT
   * $CROWDWIZD_PLUGINS
   * $CROWDWIZD_REPLAY
   * $CROWDWIZD_RESYNC
   * $CROWDWIZD_P2P_ENDPOINT
   * $CROWDWIZD_WITNESS_ID
   * $CROWDWIZD_PRIVATE_KEY 
   * $CROWDWIZD_TRACK_ACCOUNTS
  * $CROWDWIZD_PARTIAL_OPERATIONS
  * $CROWDWIZD_MAX_OPS_PER_ACCOUNT
   * $CROWDWIZD_ES_NODE_URL
   * $CROWDWIZD_ES_START_AFTER_BLOCK
   *  $CROWDWIZD_TRUSTED_NODE

> Значение env можно задавать в docker-compose.yml
```  
environment:
           CROWDWIZD_TRUSTED_NODE = $CROWDWIZD_TRUSTED_NODE
		    ...
```

### Мониторинг Node

Каждый docker-compose включает в себя Container Advisor и Node-exporter

* Container Advisor - анализирует и предоставляет данные об использовании ресурсов и производительности из запущенных контейнеров

*  Node exporter - метрики, связанных с утилизацией оборудованием и ядром Linux host


### Запуск проекта

```
 git clone git@gitlab.com:crowdwiz/send_message.git
 cd ./send_message && docker-compose up -d 
```

