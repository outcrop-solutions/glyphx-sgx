
#include "AwsManager.h"
//#include "aws/core/Aws.h"
//#include "aws/s3/S3Client.h"
//#include "aws/core/auth/AWSCredentialsProvider.h"
//#include "aws/s3/model/ListObjectsRequest.h"
//#include "aws/s3/model/GetObjectRequest.h"
//#include "aws/core/utils/StringUtils.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>

namespace GlyphEd
{
	//const Aws::String accessKey = "AKIAJ34ZO27XXF4KRB4A";
	//const Aws::String secretKey = "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";

	AwsManager::AwsManager() {

	}

	AwsManager::~AwsManager() {

	}

	void AwsManager::ConfigureClient() {

		/*Aws::SDKOptions options;
		Aws::InitAPI(options);

		Aws::Auth::AWSCredentials credentials;
		credentials.SetAWSAccessKeyId(accessKey);
		credentials.SetAWSSecretKey(secretKey);

		Aws::Client::ClientConfiguration config;
		config.scheme = Aws::Http::Scheme::HTTPS;
		//config.connectTimeoutMs = 10000;
		//config.requestTimeoutMs = 120000;

		Aws::S3::S3Client s3_client(credentials, config);

		{
			std::string bucket_name = "sgx-app-user-storage";
			//Aws::String region = "us-east-1";

			QFile file;
			file.setFileName("S3Log.txt");
			file.open(QIODevice::WriteOnly);
			QTextStream out(&file);
			out << "Listing Objects \n" << endl;

			Aws::S3::Model::ListObjectsRequest* listObject = new Aws::S3::Model::ListObjectsRequest();
			Aws::S3::Model::ListObjectsOutcome objects = s3_client.ListObjects(listObject->WithBucket(bucket_name.c_str()));

			for (const auto& object : objects.GetResult().GetContents()) {

				Aws::String name = object.GetKey();

				std::size_t pos = name.find(Aws::String("public"));
				bool found = (pos != Aws::String::npos);

				if (found && name.find('.') != Aws::String::npos) {
					out << object.GetKey().c_str() << endl;
				}
			}

		}
		Aws::ShutdownAPI(options);*/
	}

}
