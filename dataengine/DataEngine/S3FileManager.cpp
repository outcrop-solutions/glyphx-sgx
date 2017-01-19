#include "S3FileManager.h"
#include <boost/algorithm/string.hpp>
#include "aws/core/Aws.h"
#include "aws/s3/S3Client.h"
#include "aws/core/auth/AWSCredentialsProvider.h"
#include "aws/s3/model/ListObjectsRequest.h"
#include "aws/core/utils/StringUtils.h"
#include <algorithm>

namespace DataEngine
{
	const Aws::String accessKey = "AKIAJ34ZO27XXF4KRB4A";
	const Aws::String secretKey = "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";

	S3File::S3File(std::string bucket, Aws::S3::Model::Object obj) :
		name(obj.GetKey().substr(obj.GetKey().find_last_of("/\\")+1).c_str()),
		tag(obj.GetETag().c_str()), size(obj.GetSize()), timestamp(obj.GetLastModified().SecondsWithMSPrecision()),
		url("https://s3.amazonaws.com/" + bucket + '/' + obj.GetKey().c_str())
	{
		if (name.find("/") != std::string::npos){
			std::vector<std::string> strs;
			boost::split(strs, name, boost::is_any_of("/"));
			name = strs.at(1);
		}
	}

	S3FileManager::S3FileManager(){

		Aws::SDKOptions options;
		Aws::InitAPI(options);

		Aws::Auth::AWSCredentials credentials;
		credentials.SetAWSAccessKeyId(accessKey);
		credentials.SetAWSSecretKey(secretKey);

		Aws::Client::ClientConfiguration clientConfig;
		clientConfig.scheme = Aws::Http::Scheme::HTTPS;
		clientConfig.connectTimeoutMs = 30000;
		clientConfig.requestTimeoutMs = 600000;

		s3Client = new Aws::S3::S3Client(credentials, clientConfig);
		
	}

	S3FileManager::~S3FileManager(){

	}

	std::vector<S3File*> S3FileManager::GetFilesFromDirectory(std::string bucket, const char* directory){

		std::vector<S3File*> results;

		Aws::S3::Model::ListObjectsRequest* listObject = new Aws::S3::Model::ListObjectsRequest();
		Aws::S3::Model::ListObjectsOutcome objects = s3Client->ListObjects(listObject->WithBucket(bucket.c_str()));

		for (const auto& object : objects.GetResult().GetContents()){

			Aws::String name = object.GetKey();

			std::size_t pos = name.find(Aws::String(directory));
			bool found = (pos != Aws::String::npos);

			if (found && name.find('.') != Aws::String::npos){
				results.push_back(new S3File(bucket, object));
			}
		}
		return results;
	}

}
