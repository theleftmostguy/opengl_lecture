#include <unordered_map>
#include <vector>
#include <assert.h>

// only needed for main() aka. the test code
#include <iostream>

class Vertex {
private:
	static size_t rot (size_t x) {
		// rotate the bit string by a large prime to avoid collissions
		const size_t PRIME = 29;
		return x<<PRIME | x>>((sizeof(size_t)*8)-PRIME);
	}
	// fuse a float into the hash value
	static void consider (size_t& h, float x) {
		static_assert(sizeof(int) == sizeof(float), "Can't do bit magic, interpreting floats as ints if they have different sizes.");
		h = rot(h) ^ reinterpret_cast<int&>(x);
	}
public:
	// change these to contain what you want, then change "hash" and "operator==" accordingly
	float x,y,z;
	float nx,ny,nz;
	Vertex (float a,float b,float c,float na,float nb,float nc) : x(a), y(b), z(c), nx(na), ny(nb), nz(nc) {}
	size_t hash() const {
		size_t ret = 0; // any constant would do
		consider(ret,x);
		consider(ret,y);
		consider(ret,z);
		consider(ret,nx);
		consider(ret,ny);
		consider(ret,nz);
		return ret;
	}
	bool operator==(const Vertex& other) const {
		return x==other.x && y==other.y && z==other.z
			&& nx==other.nx && ny==other.ny && nz==other.nz;
	}
	bool operator!=(const Vertex& other) const {
		return !(*this == other);
	}
};

// these are needed to make the unordered_map consider values of pointers
struct vertex_deref_hash {
	size_t operator()(const Vertex* v) const {
		return v->hash();
	}
};
struct vertex_deref_eq {
	bool operator()(const Vertex* a, const Vertex* b) const {
		return *a == *b;
	}
};

class VertexUnifier {
private:
	size_t next_idx; // == current number of unique vertices
	// hashmap: vertices to indices in the final array
	std::unordered_map<Vertex*,size_t,vertex_deref_hash,vertex_deref_eq> map;
	bool array_generated; // has the array been generated yet?
public:
	VertexUnifier() : next_idx(0), map(), array_generated(false) {}
	// Calculates the index position of the given vertex in the to-be-generated vertices array.
	size_t assignIdx(Vertex& vert, const bool deleteIfPresent = false) {
		assert (!array_generated); // First put all vertices, then generate the array.
		auto search = map.find(&vert);
		if (search == map.end()) { // not inside yet
			size_t this_idx = next_idx;
			next_idx++;
			map.insert(std::make_pair(&vert,this_idx));
			return this_idx;
		} else { // already there
			if (deleteIfPresent) {
				delete &vert;
			}
			return search->second;
		}
	}
	// Generates the array fitting the previously calculated new array indices.
	// Do this only once, when all your vertices have been assigned a new id.
	std::vector<Vertex*>* getVertexArray() {
		assert (!array_generated);
		array_generated = true;
		const size_t length = next_idx; // just a renaming for readability
		std::vector<Vertex*>* ret = new std::vector<Vertex*>(length);
		for (auto it=map.begin(); it!=map.end(); ++it) {
			// put the vertex to the previously assigned position
			(*ret)[it->second] = it->first;
		}
		return ret;
	}
};

class Triangle {
public:
	Triangle(size_t v1,size_t v2,size_t v3) : a(v1), b(v2), c(v3) {}
	// indices of vertices
	size_t a,b,c;
};

int main () {
	std::vector<Vertex> vertices;
	// some vertices, where vertices[0] == vertices[2]
	// these vertices will be joined
	vertices.push_back(Vertex(0,2,3,4,5,6));
	vertices.push_back(Vertex(1,2,8,4,5,9));
	vertices.push_back(Vertex(0,2,3,4,5,6));
	vertices.push_back(Vertex(3,2,8,4,5,6));
	std::vector<Triangle> triangles;
	// indices in the old "vertices"
	triangles.push_back(Triangle(0,1,2));
	triangles.push_back(Triangle(1,2,3));
	VertexUnifier vertun;
	// translate indices to new array
	for (auto it=triangles.begin(); it!=triangles.end(); ++it) {
		it->a = vertun.assignIdx(vertices.at(it->a));
		it->b = vertun.assignIdx(vertices.at(it->b));
		it->c = vertun.assignIdx(vertices.at(it->c));
	}
	#ifdef NDEBUG
	#error "The tests only work with assertions enabled."
	#endif
	// check indices correct
	assert (triangles[0].a == 0);
	assert (triangles[0].b == 1);
	assert (triangles[0].c == 0);
	assert (triangles[1].a == 1);
	assert (triangles[1].b == 0);
	assert (triangles[1].c == 2);
	// generate array corresponding to the new indices
	std::vector<Vertex*>* optimized_vertices = vertun.getVertexArray();
	// check equalities of the new vertices with the corresponding old ones
	assert (optimized_vertices->size() == 3);
	assert (*(optimized_vertices->at(0)) == vertices[0]);
	assert (*(optimized_vertices->at(0)) == vertices[2]);
	assert (*(optimized_vertices->at(1)) == vertices[1]);
	assert (*(optimized_vertices->at(2)) == vertices[3]);
	// success
	std::cout << "All tests passed." << std::endl;
	return 0;
}

