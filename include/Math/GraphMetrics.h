#pragma once

namespace Data {
class Graph;
}

namespace MathModule {

class GraphMetrics
{
public:
	static void computeGraphMetrics( Data::Graph* activeGraph );
};
}
