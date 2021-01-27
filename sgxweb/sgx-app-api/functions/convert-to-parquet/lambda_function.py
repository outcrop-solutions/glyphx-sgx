import boto3
import json
import pandas as pd
#from fastparquet import write, ParquetFile

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
    df_csv = pd.read_csv(obj['Body'])
    df_csv.head() # Test your intermediate value
    df_csv.columns = df_csv.columns.str.replace(" ", "_")
    
    #output_url = 's3://sgx-app-user-storage/private/'+identity+'/'+directory+'/'+directory
    #df_csv.to_parquet('/tmp/'+directory, compression="GZIP")
    df_csv.to_parquet('/tmp/'+directory+'.parquet', compression='GZIP')

    s3.upload_file('/tmp/'+directory+'.parquet', Bucket='sgx-app-user-storage', Key='private/'+identity+'/'+directory+'/'+directory+'.parquet')
    
    return {
        'statusCode': 200,
        'body': json.dumps('Complete')
    }
