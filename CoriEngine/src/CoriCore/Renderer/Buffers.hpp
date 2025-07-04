#pragma once
#include "GraphicsAPIs.hpp"

namespace Cori {
	enum class DRAW_TYPE {
		STATIC,
		DYNAMIC
	};

	enum class ShaderDataType {
		None = 0,
		Float, Vec2, Vec3, Vec4, Mat3, Mat4,
		Int, Int2, Int3, Int4,
		UInt, UInt2, UInt3, UInt4,
		Bool
	};

	static constexpr size_t ShaderDataTypeSize(ShaderDataType type) {
		constexpr size_t sizes[] = {
			sizeof(void*),                   // None
			sizeof(float),                   // Float
			sizeof(float) * 2,               // Vec2
			sizeof(float) * 3,               // Vec3
			sizeof(float) * 4,               // Vec4
			sizeof(float) * 3 * 3,           // Mat3
			sizeof(float) * 4 * 4,           // Mat4
			sizeof(int),                     // Int 
			sizeof(int) * 2,                 // Int2
			sizeof(int) * 3,                 // Int3
			sizeof(int) * 4,                 // Int4
			sizeof(uint32_t),                // UInt
			sizeof(uint32_t) * 2,            // UInt2
			sizeof(uint32_t) * 3,            // UInt3
			sizeof(uint32_t) * 4,            // UInt4
			sizeof(bool),                    // Bool
		};

		static_assert(sizeof(sizes) / sizeof(size_t) == (size_t)ShaderDataType::Bool + 1, "ShaderDataTypeSize: Size array is out of sync with ShaderDataType enum");

		if ((size_t)type >= (sizeof(sizes) / sizeof(size_t)))
		{
			CORI_CORE_ASSERT_ERROR(false, "ShaderDataTypeSize: Unknown shader data type");
			return 0;
		}

		return sizes[(size_t)type];
	}

	static constexpr size_t ShaderDataTypeComponentCount(ShaderDataType type) {
		constexpr size_t sizes[] = {
			0,            // None
			1,            // Float
			2,            // Vec2
			3,            // Vec3
			4,            // Vec4
			3 * 3,        // Mat3
			4 * 4,        // Mat4
			1,            // Int
			2,            // Int2
			3,            // Int3
			4,            // Int4
			1,            // UInt
			2,            // UInt2
			3,            // UInt3
			4,            // UInt4
			1,            // Bool
		};

		static_assert(sizeof(sizes) / sizeof(size_t) == (size_t)ShaderDataType::Bool + 1, "ShaderDataTypeComponentCount: Size array is out of sync with ShaderDataType enum");

		if ((size_t)type >= (sizeof(sizes) / sizeof(size_t)))
		{
			CORI_CORE_ASSERT_ERROR(false, "Unknown shader data type");
			return 0;
		}

		return sizes[(size_t)type];
	}

	class VBElement { 
	public:
		VBElement(ShaderDataType type, const std::string& name, const bool normalized = false) : m_Name(name), m_Type(type), m_Normalized(normalized), m_Size(ShaderDataTypeSize(type)), m_RuntimeID(s_NextRuntimeID.fetch_add(1, std::memory_order_relaxed)) {}

		std::string m_Name;
		ShaderDataType m_Type;
		bool m_Normalized{ false };
		uint32_t m_Size;
		uint32_t m_Offset{ 0 };

		size_t GetComponentCount() const { return ShaderDataTypeComponentCount(m_Type); }

		bool operator==(const VBElement& other) const {
			return m_RuntimeID == other.m_RuntimeID;
		}

		bool operator!=(const VBElement& other) const {
			return m_RuntimeID != other.m_RuntimeID;
		}

		VBElement(const VBElement& other)
			: m_Name(other.m_Name),
			m_Type(other.m_Type),
			m_Normalized(other.m_Normalized),
			m_Size(other.m_Size),
			m_Offset(other.m_Offset),
			m_RuntimeID(s_NextRuntimeID.fetch_add(1, std::memory_order_relaxed))
		{ }

		VBElement(VBElement&& other) noexcept
			: m_Name(std::move(other.m_Name)),
			m_Type(other.m_Type),
			m_Normalized(other.m_Normalized),
			m_Size(other.m_Size),
			m_Offset(other.m_Offset),
			m_RuntimeID(other.m_RuntimeID)
		{ }

		VBElement& operator=(VBElement&& other) noexcept {
			if (this != &other) {
				m_Name = std::move(other.m_Name);
				m_Type = other.m_Type;
				m_Normalized = other.m_Normalized;
				m_Size = other.m_Size;
				m_Offset = other.m_Offset;
			}

			return *this;
		}

	private:
		const uint32_t m_RuntimeID{ 0 };
		inline static std::atomic<uint32_t> s_NextRuntimeID{ 1 };
	};

	class VBLayout {
	public:
		VBLayout() = default;

		VBLayout(const std::initializer_list<VBElement>& elements) : m_Elements(elements) {
			for (auto& element : m_Elements) {
				element.m_Offset = m_Stride;
				m_Stride += element.m_Size;
			}
		}

		VBLayout(const VBLayout& other)
			: m_Elements(other.m_Elements),
			m_Stride(other.m_Stride)
		{ }

		VBLayout& operator=(const VBLayout& other) {
			if (this == &other) {
				return *this;
			}

			m_Elements.clear();
			m_Elements.reserve(other.m_Elements.size());
			
			for (const auto& src_element : other.m_Elements) {
				m_Elements.emplace_back(src_element);
			}

			m_Stride = other.m_Stride;

			return *this;
		}

		VBLayout(VBLayout&& other) noexcept
			: m_Elements(std::move(other.m_Elements)),
			m_Stride(other.m_Stride)
		{
			other.m_Stride = 0;
		}

		VBLayout& operator=(VBLayout&& other) noexcept {
			if (this == &other) {
				return *this;
			}

			m_Elements = std::move(other.m_Elements);
			m_Stride = other.m_Stride;
			other.m_Stride = 0;

			return *this;
		}

		inline uint32_t GetStrinde() const { return m_Stride; }
		inline const std::vector<VBElement>& GetElements() const { return m_Elements; }
		
		inline VBElement& front() { return m_Elements.front(); }
		inline const VBElement& front() const { return m_Elements.front(); }
		inline VBElement& back() { return m_Elements.back(); }
		inline const VBElement& back() const { return m_Elements.back(); }

		inline std::vector<VBElement>::iterator begin() { return m_Elements.begin(); }
		inline std::vector<VBElement>::iterator end() { return m_Elements.end(); }
		inline std::vector<VBElement>::const_iterator begin() const { return m_Elements.begin(); }
		inline std::vector<VBElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		std::vector<VBElement> m_Elements;
		uint32_t m_Stride{ 0 }; 
	};

	class VertexBuffer {
	public:
		virtual ~VertexBuffer() = default;
		virtual void Init(const float* vertices, uint32_t size, DRAW_TYPE drawtype) = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetLayout(const VBLayout& layout) = 0;
		virtual const VBLayout& GetLayout() const = 0;

		virtual void SetData(const void* data, uint32_t size) const = 0;

		static std::shared_ptr<VertexBuffer> Create();
	};

	class IndexBuffer {
	public:
		virtual ~IndexBuffer() = default;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static std::shared_ptr<IndexBuffer> Create(uint32_t* indices, uint32_t count);

	};
}