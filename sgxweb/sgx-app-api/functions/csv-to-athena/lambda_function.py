import boto3
import json
import pandas as pd

def lambda_handler(event, context):
    # TODO implement
    body = json.loads(event)
    #identity = event['identity']
    #directory = event['directory']
    identity = body['identity']
    directory = body['directory']
    filename = '/'+directory+'/'+directory+'.csv'

    s3 = boto3.client('s3')
    obj = s3.get_object(Bucket='sgx-app-user-storage', Key='private/'+identity+filename)
    df = pd.read_csv(obj['Body'], nrows=500, keep_default_na=False)

    fields = []
    #types = object, int64, float64, bool, datetime64, timedelta[ns], category
    for name, dtype in df.dtypes.iteritems():
        if dtype == 'object':
            fields.append({'field': name, 'type': 'string'})
        elif dtype == 'float64':
            fields.append({'field': name, 'type': 'double'})
        elif dtype == 'int64':
            fields.append({'field': name, 'type': 'bigint'})
        else:
            fields.append({'field': name, 'type': 'string'})
    #print(fields)
    tablename = directory.lower().replace(" ", "_")
    query = "CREATE EXTERNAL TABLE IF NOT EXISTS `"+identity+"`.`"+tablename+"` ("
    query += "`" + ''.join(e for e in str(fields[0]["field"]).replace(" ", "_") if e.isalnum() or e == '_').lower()+"` "+fields[0]["type"]
    for i in range(1, len(fields)):
        query += ", `"+''.join(e for e in str(fields[i]["field"]).replace(" ", "_") if e.isalnum() or e == '_').lower()+"` "+fields[i]["type"]
    query += ') '
    query += "STORED AS PARQUET "
    query += "LOCATION 's3://sgx-app-user-storage/private/"+identity+"/"+directory+"/' "
    query += "TBLPROPERTIES ('parquet.compression'='GZIP');" 
    
    output = 's3://sgx-athena-query-output-log/'+identity+'/';
    #return { 'statusCode': 200, 'body': query } 
    athena = boto3.client('athena')
    response = athena.start_query_execution(
        QueryString=query,
        ResultConfiguration={
            'OutputLocation': output,
            'EncryptionConfiguration': {
                'EncryptionOption': 'SSE_S3'
            }
        }
    )

    #Only needed the CSV up until table is created. All data will be read from the parquet file from here
    remove_response = s3.delete_object(Bucket='sgx-app-user-storage', Key='private/'+identity+filename)
    
    #context.succeed(fields)
    return { 'statusCode': 200, 'body': json.dumps(response['QueryExecutionId']) } 
