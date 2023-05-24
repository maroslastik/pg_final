#include   "ViewerWidget.h"

bool compareEdgesByStartY(const EDGE& e1, const EDGE& e2);
bool compareEdgesByStartX(const EDGE& e1, const EDGE& e2);
bool compareEdgesByStartYX(const QPoint& p1, const QPoint& p2);

ViewerWidget::ViewerWidget(QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
		z_buffer = new double[width() * height()];
		std::fill_n(z_buffer, width() * height(), -std::numeric_limits<double>::max());
	}
}

ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;
}

void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete painter;
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	setDataPtr();
	update();

	return true;
}

bool ViewerWidget::isEmpty()
{
	if (img == nullptr) {
		return true;
	}

	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

bool ViewerWidget::changeSize(int width, int height)
{
	QSize newSize(width, height);

	if (newSize != QSize(0, 0)) {
		if (img != nullptr) {
			delete painter;
			delete img;
		}

		img = new QImage(newSize, QImage::Format_ARGB32);
		if (!img) {
			return false;
		}
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
		update();
	}

	return true;
}

void ViewerWidget::setPixels_c(int x, int y, const QColor& color)
{
	setPixel(  x,  y, color);
	setPixel(  x, -y, color);
	setPixel( -x, -y, color);
	setPixel( -x,  y, color);
	setPixel(  y,  x, color);
	setPixel(  y, -x, color);
	setPixel( -y, -x, color);
	setPixel( -y,  x, color);
}

void ViewerWidget::drawLineBres(QPoint start, QPoint end, QColor color)
{
	double m;
	if ((double)(end.x() - start.x())==0) m = DBL_MAX;
	else m = (end.y() - start.y()) / (double)(end.x() - start.x());

	//riadiaca os je y
	if (m >= 1)
	{
		if (start.y() > end.y())
			return;
			//swap_points(start, end);

		int k1 = 2 * (end.x() - start.x()),
			k2 = 2 * (end.x() - start.x() - end.y() + start.y()),
			p = 2 * (end.x() - start.x()) - end.y() + start.y();

		int i[2] = { start.x(), start.y() };
		setPixel(i[0], i[1], (double)color.red(), (double)color.green(), (double)color.blue(), (double)color.alpha());

		while (i[1] < end.y())
		{
			i[1]++;
			if (p > 0)
			{
				i[0]++;
				p += k2;
			}
			else
			{
				p += k1;
			}
			setPixel(i[0], i[1], (double)color.red(), (double)color.green(), (double)color.blue(), (double)color.alpha());
		}
	}
	else if (m <= -1)
	{
		if (start.y() > end.y())
			return;
			//swap_points(start, end);

		int k1 = 2 * (end.x() - start.x()),
			k2 = 2 * (end.x() - start.x() + end.y() - start.y()),
			p = 2 * (end.x() - start.x()) + end.y() - start.y();

		int i[2] = { start.x(), start.y() };
		setPixel(i[0], i[1], (double)color.red(), (double)color.green(), (double)color.blue(), (double)color.alpha());

		while (i[1] < end.y())
		{
			i[1]++;
			if (p < 0)
			{
				i[0]--;
				p += k2;
			}
			else
			{
				p += k1;
			}
			setPixel(i[0], i[1], (double)color.red(), (double)color.green(), (double)color.blue(), (double)color.alpha());
		}
	}
	else if (m >= 0 && m < 1)
	{
		if (start.x() > end.x())
			return;
		//swap_points(start, end);

		int k1 = 2 * (end.y() - start.y()),
			k2 = 2 * (end.y() - start.y() - end.x() + start.x()),
			p = 2 * (end.y() - start.y()) - end.x() + start.x();

		int i[2] = { start.x(), start.y() };
		setPixel(i[0], i[1], (double)color.red(), (double)color.green(), (double)color.blue(), (double)color.alpha());

		while (i[0] < end.x())
		{
			i[0]++;
			if (p > 0)
			{
				i[1]++;
				p += k2;
			}
			else
			{
				p += k1;
			}
			setPixel(i[0], i[1], (double)color.red(), (double)color.green(), (double)color.blue(), (double)color.alpha());
		}
	}
	else
	{
		if (start.x() > end.x())
			return;
			//swap_points(start, end);

		int k1 = 2 * (end.y() - start.y()),
			k2 = 2 * (end.y() - start.y() + end.x() - start.x()),
			p = 2 * (end.y() - start.y()) + end.x() - start.x();

		int i[2] = { start.x(), start.y() };
		setPixel(i[0], i[1], (double)color.red(), (double)color.green(), (double)color.blue(), (double)color.alpha());

		while (i[0] < end.x())
		{
			i[0]++;
			if (p < 0)
			{
				i[1]--;
				p += k2;
			}
			else
			{
				p += k1;
			}
			setPixel(i[0], i[1], (double)color.red(), (double)color.green(), (double)color.blue(), (double)color.alpha());
		}
	}

	update();
}

QVector<QPoint> ViewerWidget::trim_line()
{
	if (isInside(polygon[0].x(), polygon[0].y()) && isInside(polygon[1].x(), polygon[1].y()))
	{
		return polygon;
	}

	int width = this->width()-1;
	int height = this->height()-1;
	double tl = 0, tu = 1;
	QPoint D(polygon[1] - polygon[0]);
	QPoint E[] = { QPoint(0, 0), QPoint(0, height), QPoint(width, height), QPoint(width, 0) };
	QVector<QPoint> new_line;
	new_line.resize(2);

	for (int i = 0; i < 4; i++)
	{
		QPoint W = (polygon[0]) - (E[i]);
		QPoint N = QPoint(E[(i+1)%4].y() - E[i].y(), E[i].x() - E[(i + 1) % 4].x());
		double DdotN = QPoint::dotProduct(D, N);
		if (DdotN != 0)
		{
			double t = -QPoint::dotProduct(N,W) / DdotN;

			if (DdotN > 0 && t <= 1) 
			{
				if (tl < t)
					tl = t;
			}
			else if(DdotN < 0 && t >= 0)
			{
				if (tu > t)
					tu = t;
			}
		}	
	}
	
	if (tl == 0 && tu == 1)  return polygon;
	else if (tl < tu)
	{
		new_line[0] = polygon[0] + D * tl;
		new_line[1] = polygon[0] + D * tu;
	}
	// ked presiahnem hranicu x=0 alebo y=0 tak su hodnoty rovnake, ak x=xmax, y=ymax tak tl=0 tu=1
	return new_line;
}

QVector<VERTEX> ViewerWidget::trim_polygon(QVector<VERTEX> W)
{
	QVector<VERTEX>	E = {
			VERTEX(0, 0, 0),
			VERTEX(width() - 1, 0, 0),
			VERTEX(width() - 1, height() - 1, 0),
			VERTEX(0, height() - 1, 0) };

	for (int i = 0; i < 4; i++)
	{
		W = trim_left_side(E[i].x,W);

		for (int j = 0; j < W.size(); j++)
		{
			W[j] = VERTEX{ W[j].y, -W[j].x,0 };
		}
		for (int j = 0; j < E.size(); j++)
		{
			E[j] = VERTEX{ E[j].y, -E[j].x,0 };
		}
	}
	return W;
}

QVector<VERTEX> ViewerWidget::trim_left_side(int xmin, QVector<VERTEX> V)
{
	int	n = V.size();
	VERTEX S = V[n - 1];
	QVector<VERTEX> W = {};
	for (int i = 0; i < n; i++)
	{
		if (V[i].x >= xmin)
		{
			if (S.x >= xmin)
			{
				W.push_back(V[i]);
			}
			else
			{
				VERTEX P = VERTEX{
					xmin,
					S.y + static_cast<int>((xmin - S.x) * ((double)(V[i].y - S.y) / (V[i].x - S.x)) + 0.5),0};
				W.push_back(P);
				W.push_back(V[i]);
			}
		}
		else
		{
			if (S.x >= xmin)
			{
				VERTEX P = VERTEX{
					xmin,
					S.y + static_cast<int>((xmin - S.x) * ((double)(V[i].y - S.y) / (V[i].x - S.x)) + 0.5),0 };
				W.push_back(P);
			}
		}
		S = V[i];
	}
	return W;
}

void ViewerWidget::updateZBuffer(int x, int y, double z)
{
	int index = y * width() + x;
	if (x >= 0 && x < width() && y >= 0 && y < height())
	{
		if (z < z_buffer[index])
		{
			z_buffer[index] = z;
		}
	}
}

void ViewerWidget::fill_polygon(QVector<VERTEX> polygon, QColor color)
{
	if (polygon.size() == 2 || polygon.size() == 1)
	{
		return;
	}
	else if (polygon.size() == 3)
	{
		fill_triangle(polygon, color);
		return;
	}
	QVector<VERTEX> T = polygon;
	QVector<EDGE> edges;

	// Setting up edges
	for (int i = 0; i < T.size(); i++)
	{
		VERTEX start = T[i], end = T[(i + 1) % T.size()];
		// Removing horizontal edges
		if (start.y == end.y)
			continue;
		// Handling top-down orientation
		if (start.y > end.y)
		{
			VERTEX temp = start;
			start = end;
			end = temp;
		}

		// Adjusting the end point
		end.y -= 1;

		// Setting up and adding the edge to the edges array
		EDGE edge;
		edge.start = start;
		edge.end = end;
		edge.dy = end.y - start.y;
		edge.x = (double)start.x;
		edge.w = (double)(end.x - start.x) / (double)(end.y - start.y);
		edges.push_back(edge);
	}

	// Sorting edges based on start y-coordinate
	std::sort(edges.begin(), edges.end(), compareEdgesByStartY);

	// Setting y_min and y_max
	int y_min = edges[0].start.y;
	int y_max = y_min;

	for (int i = 0; i < edges.size(); i++)
	{
		if (edges[i].end.y > y_max)
		{
			y_max = edges[i].end.y;
		}
	}

	// Creating the edge table TH
	QVector<QList<EDGE>> TH;
	TH.resize(y_max - y_min + 1);

	for (int i = 0; i < edges.size(); i++)
	{
		TH[edges[i].start.y - y_min].push_back(edges[i]);
	}

	QVector<EDGE> ZAH;
	double y = y_min;

	for (int i = 0; i < TH.size(); i++)
	{
		// Moving from TH to ZAH
		if (TH[i].size() != 0)
		{
			for (int j = 0; j < TH[i].size(); j++)
			{
				ZAH.push_back(TH[i][j]);
			}
		}

		// Sorting active edges based on x-coordinate
		std::sort(ZAH.begin(), ZAH.end(), compareEdgesByStartX);

		// Drawing lines
		for (int j = 0; j < ZAH.size(); j += 2)
		{
			if (ZAH[j].x != ZAH[j + 1].x)
			{
				drawLineDDA(
					VERTEX{ 
						static_cast<int>(ZAH[j].x + 0.5), 
						static_cast<int>(y), 
						0 }, 
					VERTEX{ 
						static_cast<int>(ZAH[j + 1].x + 0.5), 
						static_cast<int>(y), 
						0 }, color);
			}
		}

		// Updating Z-buffer and drawing lines
		for (int j = 0; j < ZAH.size(); j++)
		{
			if (ZAH[j].dy == 0)
			{
				ZAH.remove(j);
				j--;
			}
			else
			{
				// Update Z-buffer before drawing
				updateZBuffer((int)(ZAH[j].x + 0.5), (int)y, ZAH[j].start.z);
				ZAH[j].x += ZAH[j].w;
				ZAH[j].dy--;
			}
		}

		y++;
	}
}

void ViewerWidget::setPixel(int x, int y, float z, const QColor& color)
{
	if (z < z_buffer[y * width() + x])
	{
		z_buffer[y * width() + x] = z;
		if (color.isValid())
		{
			size_t startbyte = y * img->bytesPerLine() + x * 4;
			data[startbyte] = color.blue();
			data[startbyte + 1] = color.green();
			data[startbyte + 2] = color.red();
			data[startbyte + 3] = color.alpha();
		}
	}
}

void ViewerWidget::drawLineDDA(VERTEX start, VERTEX end, QColor color)
{
	double m;
	if (end.x - start.x == 0)
		m = DBL_MAX;
	else
		m = static_cast<double>(end.y - start.y) / (end.x - start.x);

	// Determine whether to iterate over x or y
	if (std::abs(m) >= 1)
	{
		if (start.y > end.y)
			swap_points(start, end);

		double x = start.x;
		double y = start.y;
		double z = start.z;
		while (y <= end.y)
		{
			setPixel(static_cast<int>(x + 0.5), static_cast<int>(y + 0.5), z, color);
			x += 1 / m;
			y++;
			z += (end.z - start.z) / (end.y - start.y);
		}
	}
	else
	{
		if (start.x > end.x)
			swap_points(start, end);

		double x = start.x;
		double y = start.y;
		double z = start.z;

		while (x <= end.x)
		{
			setPixel(static_cast<int>(x + 0.5), static_cast<int>(y + 0.5), z, color);
			x++;
			y += m;
			z += (end.z - start.z) / (end.x - start.x);
		}
	}
	update();
}

void ViewerWidget::fill_triangle(QVector<VERTEX> T, QColor color)
{
	std::sort(T.begin(), T.end(),
		[](VERTEX a, VERTEX b)
		{
			if ((int)a.y + 0.5 == (int)b.y + 0.5)
				return a.x < b.x;
			return a.y < b.y;
		});

	EDGE e1;
	EDGE e2;

	if (T[0].y == T[1].y)
	{
		// spodny trojuh
		e1.start = T[0];
		e1.end = T[2];
		e1.w = (double)(T[2].x - T[0].x) / (double)(T[2].y - T[0].y);

		e2.start = T[1];
		e2.end = T[2];
		e2.w = (double)(T[2].x - T[1].x) / (double)(T[2].y - T[1].y);
	}
	else if (T[1].y == T[2].y)
	{
		// horny trojuh
		e1.start = T[0];
		e1.end = T[1];
		e1.w = (double)(T[1].x - T[0].x) / (double)(T[1].y - T[0].y);

		e2.start = T[0];
		e2.end = T[2];
		e2.w = (double)(T[2].x - T[0].x) / (double)(T[2].y - T[0].y);
	}
	else
	{
		// rozdelime a rekurzivne vyplnime
		double m = (double)(T[2].y - T[0].y) / (double)(T[2].x - T[0].x);
		VERTEX P{ static_cast<int>((T[1].y - T[0].y) / m + T[0].x), T[1].y, 0 };
		P.z = T[0].z + (P.x - T[0].x) * ((T[2].z - T[0].z) / (T[2].x - T[0].x));

		if (T[1].x < P.x)
		{
			fill_triangle({ T[0], T[1], P }, color);
			fill_triangle({ T[1], P, T[2] }, color);
		}
		else
		{
			fill_triangle({ T[0], P, T[1] }, color);
			fill_triangle({ P, T[1], T[2] }, color);
		}
		return;
	}

	double x1 = e1.start.x;
	double x2 = e2.start.x;

	QVector3D N = QVector3D::crossProduct(
		QVector3D(e1.end.x - e1.start.x, e1.end.y - e1.start.y, e1.end.z - e1.start.z),
		QVector3D(e2.end.x - e2.start.x, e2.end.y - e2.start.y, e2.end.z - e2.start.z));

	// e1.start = e2.start = t[0]
	// e1.end = T[1]
	// e2.end = T[2]

	for (int y = e1.start.y; y < e1.end.y; y++)
	{
		if (x1 != x2)
		{
			double Z1 = e1.start.z - ((x1 - e1.start.x) * N.x() + (y - e1.start.y) * N.y()) / N.z(),
				   Z2 = e2.start.z - ((x2 - e2.start.x) * N.x() + (y - e2.start.y) * N.y()) / N.z();
			updateZBuffer(static_cast<int>(x1 + 0.5), y, Z1);
			updateZBuffer(static_cast<int>(x2 + 0.5), y, Z2);
			drawLineDDA(VERTEX{ static_cast<int>(x1 + 0.5), y, Z1 }, VERTEX{ static_cast<int>(x2 + 0.5), y, Z2 }, color);
		}
		x1 += e1.w;
		x2 += e2.w;
	}
}

double ViewerWidget::max(double& one, double& two)
{ 
	if (one > two) 
		return one; 
	else 
		return two; 
}

double ViewerWidget::min(double& one, double& two) 
{
	if (one < two) 
		return one; 
	else 
		return two; 
}

void ViewerWidget::swap_points(VERTEX& one, VERTEX& two)
{
	VERTEX tmp;
	tmp.x = one.x;
	tmp.y = one.y;
	tmp.z = one.z;
	one.x = two.x;
	one.y = two.y;
	one.z = two.z;
	two.x = tmp.x;
	two.y = tmp.y;
	two.z = tmp.z;
}

bool ViewerWidget::is_polygon_inside(QVector<QPoint> P)
{
	for (int i = 0; i < P.size(); i++)
		if (!isInside(P[i].x(), P[i].y()))
			return false;
		else
			return true;
	return false;
}

void ViewerWidget::clear()
{
	polygon.clear();
	clear_canvas();
	for (int i = 0; i < width() * height(); i++)
		z_buffer[i] = -std::numeric_limits<double>::max();
}

void ViewerWidget::clear_canvas()
{
	img->fill(Qt::white);
	for (int i = 0; i < width() * height(); i++)
		z_buffer[i] = std::numeric_limits<double>::max();
	update();
	update();
}

void ViewerWidget::setUpObject(QVector<VERTEX*> points, QVector<QVector<int>> polygons)
{
	QVector<QColor> colors = { QColor(Qt::red),QColor(Qt::green),QColor(Qt::blue),QColor(Qt::yellow),QColor(Qt::magenta),QColor(Qt::cyan) };
	int num_points = points.size();
	object.vertexes.reserve(num_points);
	object.vertexes.resize(num_points);

	for (int i = 0; i < num_points; i++)
	{
		object.vertexes[i] = points[i];
	}

	int num_faces = polygons.size();
	object.faces.reserve(num_faces);
	object.faces.resize(num_faces);

	for (int i = 0; i < num_faces; i++)
	{
		QVector<VERTEX*> n_face;
		n_face.reserve(polygons[i].size());
		n_face.resize(polygons[i].size());

		for (int j = 0; j < polygons[i].size(); j++)
		{
			n_face[j] = object.vertexes[polygons[i][j]];
		}
		object.faces[i] = new FACE{ n_face };
		object.faces[i]->f_color = colors[i%6];
	}
	moveObjectToCentre(object);
}

void ViewerWidget::moveObjectToCentre(OBJECT object)
{
	int sum_x = 0, sum_y = 0, sum_z = 0;
	for (int i = 0; i < object.vertexes.size(); i++) 
	{
		sum_x += object.vertexes[i]->x;
		sum_y += object.vertexes[i]->y;
		sum_z += object.vertexes[i]->z;
	}
	int center_x = sum_x / object.vertexes.size();
	int center_y = sum_y / object.vertexes.size();
	int center_z = sum_z / object.vertexes.size();

	for (int i = 0; i < object.vertexes.size(); i++) 
	{
		object.vertexes[i]->x -= center_x;
		object.vertexes[i]->y -= center_y;
		object.vertexes[i]->z -= center_z;
	}
	qDebug() << "presunute";
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}

bool compareEdgesByStartY(const EDGE& e1, const EDGE& e2)
{
	return e1.start.y < e2.start.y;
}

bool compareEdgesByStartX(const EDGE& e1, const EDGE& e2)
{
	return e1.start.x < e2.start.x;
}

bool compareEdgesByStartYX(const VERTEX& p1, const VERTEX& p2)
{
	if (p1.y < p2.y)
		return true;
	else if (p1.y > p2.y)
		return false;

	if (p1.x < p2.x)
		return true;
	else if (p1.x > p2.x)
		return false;

	return false;
}