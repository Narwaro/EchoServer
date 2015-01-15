#include "main.h"

using namespace std;

MYSQL* mysql () {
	srand(time(NULL));
	MYSQL_RES *mTabelle;
    MYSQL_ROW mRecord;
    int mError;
    MYSQL* mConnection = mysql_init(NULL);
	mysql_real_connect(mConnection,"localhost","main","437280","main",3306,0,0);
		
    if (mConnection == NULL)  {
        printf("Could not connect. Leaving: %s",mysql_error(mConnection));
        return NULL;
    } else {
		printf("Connected to MySQL-Server!\n");
    }
    //mysql_close(mConnection);
    
    int result = mysql_query(mConnection, "SELECT * FROM log ORDER BY id DESC LIMIT 0,10");
    if(result == 0) {
	cout << "Query succeeded.\n" << endl; 
	MYSQL_RES* res = mysql_store_result(mConnection);
	MYSQL_ROW row = mysql_fetch_row(res);
	for(int i = 0; i < mysql_num_fields(res); i++ ) {
            cout << row[i] << "|";
	}
	cout << "\n" << endl;
    } else {
	cout << "ERROR!\n" << mysql_error(mConnection) << endl;
        //return NULL;
    }
    return mConnection;
}
