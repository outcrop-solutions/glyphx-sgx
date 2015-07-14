#include "antztransformer.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <boost/filesystem.hpp>
#include "sourcedatamanager.h"
#include "antzcsvwriter.h"
#include "defaultbaseimageproperties.h"

namespace SynGlyphXANTz {

	ANTzTransformer::ANTzTransformer(const QString& baseOutputDir) :
		SynGlyphX::Transformer(),
		m_baseOutputDir(baseOutputDir)
	{

	}

	ANTzTransformer::~ANTzTransformer()
	{

	}

	const QStringList& ANTzTransformer::GetCSVFilenames() const {

		return m_csvFilenames;
	}

	const QStringList& ANTzTransformer::GetBaseImageFilenames() const {

		return m_baseImageFilenames;
	}

	void ANTzTransformer::GenerateCache(const SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& baseImageFilenameDirectory) {

		Q_FOREACH(QString csvFilename, csvFilenames) {

			if (QFile::exists(csvFilename)) {

				if (!QFile::remove(csvFilename)) {

					throw std::exception("Failed to remove old cache");
				}
			}
		}

		std::vector<ANTzGrid> grids;
		GenerateGrids(grids, mapping, baseImageFilenameDirectory);

		SynGlyphX::GlyphGraph::ConstSharedVector trees = CreateGlyphTreesFromMinMaxTrees(mapping);
		
		ANTzCSVWriter& writer = ANTzCSVWriter::GetInstance();
		writer.Write(csvFilenames[0].toStdString(), csvFilenames[1].toStdString(), trees, grids);

		m_csvFilenames = csvFilenames;
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

				const SynGlyphX::DefaultBaseImageProperties* const properties = dynamic_cast<const SynGlyphX::DefaultBaseImageProperties* const>(baseImage.GetProperties());
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
			grid.SetPosition(baseImage.GetPosition());
			grid.SetRotation(baseImage.GetRotationAngles());
			grid.SetTextureID(m_textureIDs.back());

			SynGlyphX::Vector3 gridScale;
			gridScale[0] = baseImage.GetWorldSize()[0] / 360.0;
			gridScale[1] = baseImage.GetWorldSize()[1] / 180.0;
			gridScale[2] = 1.0;
			grid.SetScale(gridScale);

			grids.push_back(grid);
		}
	}

	void ANTzTransformer::Clear() {

		m_csvFilenames.clear();
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