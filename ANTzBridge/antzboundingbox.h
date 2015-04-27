#ifndef SYNGLYPHXANTZ_ANTZBOUNDINGBOX_H
#define SYNGLYPHXANTZ_ANTZBOUNDINGBOX_H

#include "ANTzBridge.h"
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <utility>
#include <boost/optional.hpp>
#include "data/nptypes.h"

namespace SynGlyphXANTz {

	class ANTZBRIDGE_API ANTzBoundingBox
	{
	public:
		typedef std::pair<glm::vec3, glm::vec3> Line;
		typedef boost::optional<float> Intersection;

		ANTzBoundingBox();
		ANTzBoundingBox(const ANTzBoundingBox& boundingBox);
		~ANTzBoundingBox();

		ANTzBoundingBox& operator=(const ANTzBoundingBox& boundingBox);

		void SetTransform(const glm::mat4& matrix);
		const glm::mat4& GetTransform() const;

		const glm::vec3& GetMinCorner() const;
		const glm::vec3& GetMaxCorner() const;

		Intersection DoesLineIntersect(const Line& line);

		static ANTzBoundingBox CreateBoundingBox(pNPnode node, float rootGridZScale = 1.0f);
		static ANTzBoundingBox CreateBoundingBox(pNPnode node, const glm::mat4& previousTransform);

	private:
		static glm::mat4 CreateTransform(pNPnode node, float rootGridZScale = 1.0f);
		static glm::mat4 CreateTransform(pNPnode node, const glm::mat4& previousTransform);

		static ANTzBoundingBox CreateTorusBB(double ratio);
		static ANTzBoundingBox GetDefaultBB(pNPnode node);

		static void AddFinalTransform(pNPnode node, glm::mat4& transform);
		static void ApplyRotation(pNPnode node, glm::mat4& transform);
		static void ApplyCubeTransform(pNPnode node, glm::mat4& transform);
		static void ApplySphereTransform(pNPnode node, glm::mat4& transform);
		static void ApplyCylinderTransform(pNPnode node, glm::mat4& transform);
		static void ApplyPinTransform(pNPnode node, glm::mat4& transform);
		static void ApplyTorusTransform(pNPnode node, glm::mat4& transform);

		static ANTzBoundingBox CreatePinBB();
		static ANTzBoundingBox CreateCubeBB();
		static ANTzBoundingBox CreateCylinderBB();
		static ANTzBoundingBox CreateSphereBB();

		static const ANTzBoundingBox s_pin;
		static const ANTzBoundingBox s_cube;
		static const ANTzBoundingBox s_cylinder;
		static const ANTzBoundingBox s_sphere;

		glm::vec3 m_minCorner;
		glm::vec3 m_maxCorner;
		glm::mat4 m_matrixTransform;
	};

} //namespace SynGlyphXANTz

#endif //SYNGLYPHXANTZ_ANTZBOUNDINGBOX_H