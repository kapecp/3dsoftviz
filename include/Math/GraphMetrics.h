#pragma once

namespace Data
{
	class Graph;
}

namespace MathModule{

class GraphMetrics
{
public:
	void static computeGraphMetrics(Data::Graph * activeGraph);
};
}
