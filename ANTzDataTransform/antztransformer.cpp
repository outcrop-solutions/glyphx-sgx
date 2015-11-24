#include "antztransformer.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <boost/filesystem.hpp>
#include "sourcedatamanager.h"
#include "antzcsvwriter.h"
#include "defaultbaseimageproperties.h"

namespace SynGlyphXANTz {

	ANTzTransformer::ANTzTransformer(const QString& baseOutputDir, ANTzCSVWriter::OutputPlatform platform) :
		SynGlyphX::Transformer(),
		m_baseOutputDir(baseOutputDir),
		m_platform(platform)
	{

	}

	ANTzTransformer::~ANTzTransformer()
	{

	}

	const ANTzCSVWriter::FilenameList& ANTzTransformer::GetOutputFilenames() const {

		return m_outputFilenames;
	}

	const QStringList& ANTzTransformer::GetBaseImageFilenames() const {

		return m_baseImageFilenames;
	}

	void ANTzTransformer::GenerateCache(const SynGlyphX::DataTransformMapping& mapping, const ANTzCSVWriter::FilenameList& outputFiles, const QString& baseImageFilenameDirectory) {

		for (const std::string& filename : outputFiles) {

			QString qFilename = QString::fromStdString(filename);
			if (QFile::exists(qFilename)) {

				if (!QFile::remove(qFilename)) {

					throw std::exception("Failed to remove old cache");
				}
			}
		}

		std::vector<ANTzGrid> grids;
		GenerateGrids(grids, mapping, baseImageFilenameDirectory);

		SynGlyphX::GlyphGraph::ConstSharedVector trees = CreateGlyphTreesFromMinMaxTrees(mapping);
		
		ANTzCSVWriter& writer = ANTzCSVWriter::GetInstance();
		writer.Write(outputFiles, trees, grids, m_platform);

		m_outputFilenames = outputFiles;
	}

	void ANTzTransformer::GenerateGrids(std::vector<ANTzGrid>& grids, const SynGlyphX::DataTransformMapping& mapping, const QString& baseImageFilenameDirectory) {

		std::unordered_map<std::string, unsigned int> userBaseImages;
		unsigned int nextTextureID = NumberOfDefaultBaseImages + 1;
		for (const SynGlyphX::BaseImage& baseImage : mapping.GetBaseObjects()) {

			if (baseImage.GetType() == SynGlyphX::BaseImage::Type::DownloadedMap) {

				QString downloadedImageFilename = baseImageFilenameDirectory + GenerateBaseImageFilename(nextTextureID);
				if (DownloadBaseImage(mapping, baseImage, downloadedImageFilename)) {

					m_baseImageFilenames.push_back(downloadedImageFilename);
					m_textureIDs.push_back(nextTextureID);
					++nextTextureID;
				}
				else {

					//Use World Image
					m_textureIDs.push_back(1);
				}
			}
			else if (baseImage.GetType() == SynGlyphX::BaseImage::Type::UserImage) {

				QString sourceImageFilename = GetUserImageFilename(baseImage);
				std::unordered_map<std::string, unsigned int>::iterator userBaseImage = userBaseImages.find(sourceImageFilename.toStdString());
				if (userBaseImage == userBaseImages.end()) {

					m_textureIDs.push_back(nextTextureID);
					userBaseImages[sourceImageFilename.toStdString()] = nextTextureID;
					m_baseImageFilenames.push_back(baseImageFilenameDirectory + GenerateBaseImageFilename(nextTextureID++));
					CopyImage(sourceImageFilename, m_baseImageFilenames.last());
				}
				else {

					m_textureIDs.push_back(userBaseImage->second);
				}
			}
			else {

				auto properties = std::dynamic_pointer_cast<const SynGlyphX::DefaultBaseImageProperties>(baseImage.GetProperties());
				if (properties->GetDefaultBaseImageType() == SynGlyphX::DefaultBaseImageProperties::World) {

					m_textureIDs.push_back(1);
				}
				else {

					QString imageFilename = s_defaultImagesDirectory + QString::fromStdWString(SynGlyphX::DefaultBaseImageProperties::GetBasefilename(properties->GetDefaultBaseImageType()));
					std::unordered_map<std::string, unsigned int>::iterator image = userBaseImages.find(imageFilename.toStdString());
					if (image == userBaseImages.end()) {

						m_textureIDs.push_back(nextTextureID);
						userBaseImages[imageFilename.toStdString()] = nextTextureID;
						m_baseImageFilenames.push_back(baseImageFilenameDirectory + GenerateBaseImageFilename(nextTextureID++));
						CopyImage(imageFilename, m_baseImageFilenames.last());
					}
					else {

						m_textureIDs.push_back(image->second);
					}
				}
			}

			ANTzGrid grid;
			grid.SetVisible(true);
			grid.SetPosition(baseImage.GetPosition());
			grid.SetRotation(baseImage.GetRotationAngles());
			grid.SetTextureID(m_textureIDs.back());

			grid.SetSize(baseImage.GetWorldSize());

			grid.SetColor(baseImage.GetGridLinesColor());
			grid.SetSegments({ { baseImage.GetGridLineCounts()[1] + 1, baseImage.GetGridLineCounts()[0] + 1 } });

			grids.push_back(grid);
		}
	}

	void ANTzTransformer::Clear() {

		for (auto& filename : m_outputFilenames) {

			filename.clear();
		}
		m_baseImageFilenames.clear();
		m_textureIDs.clear();
	}

	QString ANTzTransformer::GenerateBaseImageFilename(unsigned int index) const {

		QString num = QString::number(index);
		QString prefix = "map";

		for (int i = 0; i < 5 - num.length(); ++i) {

			prefix += '0';
		}

		return prefix + num + ".jpg";
	}

	unsigned int ANTzTransformer::GetTextureID(unsigned int index) const {

		return m_textureIDs[index];
	}

} //namespace SynGlyphXANTz