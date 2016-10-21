#include "stdafx.h"
#include "Datasource.h"

#include "Dataset.h"

void FastRDF::Datasource::update() const
{
	Dataset->updateDatasource(DatasourceId);
}
