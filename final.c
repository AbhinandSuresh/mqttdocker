#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTAsync.h>
#include <time.h>
#include <sqlite3.h>

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID1   "ClientPub"
#define CLIENTID2   "ClientSub"
char*   TOPIC =     "log/#";
#define QOS         1
#define TIMEOUT     10000L

int disc_finished = 0;
int subscribed = 0;
int finished = 0;
char* msg;
char* logm;
char* time1;
volatile MQTTAsync_token deliveredtoken;

MQTTAsync client1;
MQTTAsync client2;
void onPublish(void* context, MQTTAsync_successData* response);
void onPublishFailure(void* context, MQTTAsync_failureData* response);

void time_convert(char* message,int* unix1,int* unix2)
{
struct tm t;
time_t t_of_day;
int hr,mint,secnd,yr,mnth,dy;
char str[100];
strcpy(str,message);
   const char s[] = "<;>";
   const char ti[]=":";
   char *token;
   char *t1;
   char *t2;
   char*oper;
   int flag=0;
   /* get the first token */
   token = strtok(str, s);
   t1=token;
   printf( "Output1= %s\n", t1 );
   token = strtok(NULL, s);
   t2=token;
   printf( "Output2= %s\n", t2 );
   while(flag<2)
   {
      if(flag==0)
   {
       oper=t1;
   }
   if(flag==1)
   {
       oper=t2;
   }
       token=strtok(oper,":");
       hr=atoi(token);
       token=strtok(NULL,":");
       mint=atoi(token);
       token=strtok(NULL,":");
       secnd=atoi(token);
       token=strtok(NULL,":");
       yr=atoi(token);
       token=strtok(NULL,":");
       mnth=atoi(token);
       token=strtok(NULL,":");
       dy=atoi(token);
        t.tm_year = yr-1900;  // Year - 1900
        t.tm_mon = mnth-1;    // Month, where 0 = jan
        t.tm_mday =dy;        // Day of the month
        t.tm_hour =hr;
        t.tm_min =mint;
        t.tm_sec =secnd;
        t.tm_isdst = -1;      // Is DST on? 1 = yes, 0 = no, -1 = unknown
        t_of_day = timegm(&t);
        printf("hour %d\n",hr);
      if(flag==0)
   {
       *unix1=(long)t_of_day;
       printf("seconds since the Epoch 1: %d\n", *unix1);
   }
   if(flag==1)
   {
       *unix2=(long)t_of_day;
       printf("seconds since the Epoch 2: %d\n",*unix2);
   }
   flag++;
   }
}

void stringParse(char* mesg){
char inMsg[500];
strcpy(inMsg,mesg);
char delim[]="<;>";
char *ptr = strtok(inMsg, delim);
time1 = ptr;
int flag=1;
        while(ptr !=NULL)
        {
          ptr = strtok(NULL,delim);
          flag++;
          if(flag==2)
          {
                msg=ptr;
          }
          if (flag==3)
          {
                logm=ptr;
          }
        }
}

int sqlcallback(void *NotUsed, int argc, char **argv, char **azColName)
{
        NotUsed = 0;
        int rc;
        char Payload[500];printf("received query responses .. \n");
    for (int i = 0; i < argc; i++)
    {
        sprintf(Payload,"%s = %s", azColName[i], argv[i] ? argv[i] :"NULL");
        printf("%s\n",Payload);
        MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
        MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
        pubmsg.payload = Payload;
        pubmsg.payloadlen = strlen(pubmsg.payload);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;
        deliveredtoken = 0;
        opts.onSuccess = onPublish;
        opts.onFailure = onPublishFailure;
        opts.context = client1;
        deliveredtoken = 0;
                if ((rc = MQTTAsync_sendMessage(client1, "callbackmsg", &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
                {
                printf("Failed to start sendMessage, return code %d\n", rc);
                exit(EXIT_FAILURE);
		}
        }
printf("--- query responses .. \n");
 return 0;
}
int sqlRetriveData(int t1,int t2)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open("sensordb.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }
    char sql[500];
        sprintf(sql,"SELECT * FROM Sensors where UTCtime >=%d and  UTCtime <= %d ;",t1,t2);
        printf("\n%s\n",sql);
        rc = sqlite3_exec(db, sql, sqlcallback, 0, &err_msg);
        if (rc != SQLITE_OK )
        {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
        }
return 0;
}
int sqlInsert (char *logTime ,char* SensorName, char* logm) {
    time_t utcT;
    time(&utcT);
    char* hrT;
    hrT=ctime(&utcT);
    sqlite3 *db;
    char *err_msg = 0;
    char sqlQ[500];
    int rc = sqlite3_open("sensordb.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        }
        sprintf(sqlQ,"CREATE TABLE IF NOT EXISTS Sensors(UTCtime LONG, HRtime TEXT,SensorName TEXT,logMgs TEXT);" "INSERT INTO Sensors(UTCtime,HRtime,SensorName,logMgs)VALUES(%ld,\'%s\',\'%s\',\'%s\');",utcT,logTime,SensorName,logm);
        rc = sqlite3_exec(db, sqlQ, 0, 0, &err_msg);
        if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
        }
 return 0;
}
int recievedMgs(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{ 
        int i;
        int t1,t2;
        char* payloadptr;
        printf("Message arrived\n");
        printf("     topic: %s\n", topicName);
        payloadptr = message->payload;
        if (!strcmp(topicName,"log/insert"))
        {
          	stringParse(payloadptr);
          	char msg1[20];
          	char logs[500];
          	char time2[100];
          	strcpy(msg1,msg);
          	strcpy(logs,logm);
          	strcpy(time2,time1);
        	sqlInsert(time2,msg1,logs);
        }
        //insert data to sql db
        if (!strcmp(topicName,"log/retrive"))
        {
        	time_convert(payloadptr,&t1,&t2);
		printf("t1 is %d & t2 is %d\n",t1,t2);
		printf("Retriving Datas...\n#########################");
        	sqlRetriveData(t1,t2);
		printf("\n########################\n");
        }
        MQTTAsync_freeMessage(&message);
        MQTTAsync_free(topicName);
        return 1;
		//retrive data from sql db
}
void onDisconnect(void* context, MQTTAsync_successData* response)
{
        printf("Successfully disconnected\n");
        disc_finished = 1;
}
void onSubscribe(void* context, MQTTAsync_successData* response)
{
        printf("Subscribe succeeded\n");
        subscribed = 1;
}
void onPublish(void* context, MQTTAsync_successData* response)
{
        printf(".");
}
void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
        printf("Subscribe failed, rc %d\n", response ? response->code : 0);
        finished = 1;
}
void onPublishFailure(void* context, MQTTAsync_failureData* response)
{
        printf("publish failed, rc %d\n", response ? response->code : 0);
}
void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
        printf("Connect failed, rc %d\n", response ? response->code : 0);
        finished = 1;
}
void onConnect(void* context, MQTTAsync_successData* response)
{
        MQTTAsync client2 = (MQTTAsync)context;
        MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
        MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
        int rc;
        printf("Subscribing Client Successfully connected\n");
        printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
        "***************\n\nEnter Q to quit\n\n***************\n", TOPIC, CLIENTID2,QOS);
        opts.onSuccess = onSubscribe;
        opts.onFailure = onSubscribeFailure;
        opts.context = client2;
        deliveredtoken = 0;
        if ((rc = MQTTAsync_subscribe(client2,TOPIC,QOS , &opts)) != MQTTASYNC_SUCCESS)
        {
                printf("Failed to start subscribe, return code %d\n", rc);
                exit(EXIT_FAILURE);
        }
}
void onConnect1(void* context, MQTTAsync_successData* response)
{
        MQTTAsync client1 = (MQTTAsync)context;
        printf("Publishing Client Successfully connected\n");
}
void connlost(void *context, char *cause)
{
        MQTTAsync client2 = *((MQTTAsync*)context);
        MQTTAsync_connectOptions conn_opts2 = MQTTAsync_connectOptions_initializer;
        int rc;
        printf("\nConnection lost\n");
        printf("     cause: %s\n", cause);
        printf("Reconnecting 2\n");
        conn_opts2.keepAliveInterval = 20;
        conn_opts2.cleansession = 1;
        conn_opts2.onSuccess = onConnect;
        conn_opts2.onFailure = onConnectFailure;
        conn_opts2.context = client2;

        if ((rc = MQTTAsync_connect(client2, &conn_opts2)) != MQTTASYNC_SUCCESS)
        {
        printf("Failed to start connect, return code %d\n", rc);
        finished = 1;
        }
}

int main(int argc, char* argv[])
{
	setbuf(stdout,NULL);
	printf("Welcome\nIoTunes Technologies\nDocker V:1.01\n");
	MQTTAsync_connectOptions conn_opts2 = MQTTAsync_connectOptions_initializer;
        MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
        MQTTAsync_connectOptions conn_opts1 = MQTTAsync_connectOptions_initializer;
        MQTTAsync_disconnectOptions disc_opts1 = MQTTAsync_disconnectOptions_initializer;

	int rc;
        int ch;

	MQTTAsync_create(&client1, ADDRESS, CLIENTID1, MQTTCLIENT_PERSISTENCE_NONE, NULL);
        MQTTAsync_setCallbacks(client1, &client1, connlost, NULL, NULL);
        conn_opts1.keepAliveInterval = 20;
        conn_opts1.cleansession = 1;
        conn_opts1.onSuccess = onConnect1;
        conn_opts1.onFailure = onConnectFailure;
        conn_opts1.context = client1;

	MQTTAsync_create(&client2, ADDRESS, CLIENTID2, MQTTCLIENT_PERSISTENCE_NONE, NULL);
        MQTTAsync_setCallbacks(client2, &client2, connlost, recievedMgs, NULL);
        conn_opts2.keepAliveInterval = 20;
        conn_opts2.cleansession = 1;
        conn_opts2.onSuccess = onConnect;
        conn_opts2.onFailure = onConnectFailure;
        conn_opts2.context = client2;

        if ((rc = MQTTAsync_connect(client2, &conn_opts2)) != MQTTASYNC_SUCCESS)
        {
                printf("Failed to start connect, return code %d\n", rc);
                exit(EXIT_FAILURE);
        }
        if ((rc = MQTTAsync_connect(client1, &conn_opts1)) != MQTTASYNC_SUCCESS)
        {
                printf("Failed to start connect, return code %d\n", rc);
                exit(EXIT_FAILURE);
        }

        do
        {
                ch = getchar();
        }
        while (ch!='Q' && ch != 'q');
        disc_opts.onSuccess = onDisconnect;
        disc_opts1.onSuccess = onDisconnect;

        if ((rc = MQTTAsync_disconnect(client2,&disc_opts)) != MQTTASYNC_SUCCESS)
        {
                printf("Failed to start disconnect of Sub Client, return code %d\n", rc);
                exit(EXIT_FAILURE);
        }
                if ((rc = MQTTAsync_disconnect(client1, &disc_opts1)) != MQTTASYNC_SUCCESS)
        {
                printf("Failed to start disconnect of Pub Client, return code %d\n", rc);
                exit(EXIT_FAILURE);
        }
        while   (!disc_finished)
                #if defined(WIN32) || defined(WIN64)
                        Sleep(100);
                #else
                        usleep(10000L);
                #endif
        MQTTAsync_destroy(&client2);
        MQTTAsync_destroy(&client1);
        return rc;
}
