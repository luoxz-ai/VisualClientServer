#include "Quad.h"

void NullQuad(stQuad& Quad)
{
	Quad.p[0] = glm::vec2(0, 0);
	Quad.p[1] = glm::vec2(0, 0);
	Quad.p[2] = glm::vec2(0, 0);
	Quad.p[3] = glm::vec2(0, 0);
}

void SetQuad(stQuad& Quad, float X0, float Y0, float X1, float Y1, float X2, float Y2, float X3, float Y3)
{
	Quad.p[0] = glm::vec2(X0, Y0);
	Quad.p[1] = glm::vec2(X1, Y1);
	Quad.p[2] = glm::vec2(X2, Y2);
	Quad.p[3] = glm::vec2(X3, Y3);
}

void SetQuad(stQuad& Quad, float StartX, float StartY, float Width, float Height)
{
	Quad.p[0] = glm::vec2(StartX, StartY);
	Quad.p[1] = glm::vec2(StartX, StartY + Height);
	Quad.p[2] = glm::vec2(StartX + Width, StartY + Height);
	Quad.p[3] = glm::vec2(StartX + Width, StartY);
}

void SetQuad(stQuad& Quad, float* p)
{
	if(!p) return;
	Quad.p[0] = glm::vec2(*p, *(p+1));
	Quad.p[1] = glm::vec2(*(p+2), *(p+3));
	Quad.p[2] = glm::vec2(*(p+4), *(p+5));
	Quad.p[3] = glm::vec2(*(p+6), *(p+7));
}

void SetQuad(stQuad& Quad, glm::vec2 P0, glm::vec2 P1, glm::vec2 P2, glm::vec2 P3)
{
	Quad.p[0] = P0;
	Quad.p[1] = P1;
	Quad.p[2] = P2;
	Quad.p[3] = P3;
}

void SetQuadCH(stQuad& Quad, float CenterX, float CenterY, float HalfWidth, float HalfHeight)
{
	Quad.p[0] = glm::vec2(CenterX - HalfWidth, CenterY - HalfHeight);
	Quad.p[1] = glm::vec2(CenterX - HalfWidth, CenterY + HalfHeight);
	Quad.p[2] = glm::vec2(CenterX + HalfWidth, CenterY + HalfHeight);
	Quad.p[3] = glm::vec2(CenterX + HalfWidth, CenterY - HalfHeight);
}

void SetQuadX(stQuad& Quad, stQuad FromQuad)
{
	Quad.p[0].x = FromQuad.p[0].x;
	Quad.p[1].x = FromQuad.p[1].x;
	Quad.p[2].x = FromQuad.p[2].x;
	Quad.p[3].x = FromQuad.p[3].x;
}

void SetQuadY(stQuad& Quad, stQuad FromQuad)
{
	Quad.p[0].y = FromQuad.p[0].y;
	Quad.p[1].y = FromQuad.p[1].y;
	Quad.p[2].y = FromQuad.p[2].y;
	Quad.p[3].y = FromQuad.p[3].y;
}

void SetQuadBy2Points(stQuad& Quad, float X0, float Y0, float X1, float Y1)
{
	Quad.p[0] = glm::vec2(X0, Y0);
	Quad.p[1] = glm::vec2(X0, Y1);
	Quad.p[2] = glm::vec2(X1, Y1);
	Quad.p[3] = glm::vec2(X1, Y0);
}

void MoveQuad(stQuad& Quad, float X, float Y)
{
	Quad.p[0] += glm::vec2(X, Y);
	Quad.p[1] += glm::vec2(X, Y);
	Quad.p[2] += glm::vec2(X, Y);
	Quad.p[3] += glm::vec2(X, Y);
}

void MoveQuad(stQuad& Quad, glm::vec2 Velocity)
{
	Quad.p[0] += Velocity;
	Quad.p[1] += Velocity;
	Quad.p[2] += Velocity;
	Quad.p[3] += Velocity;
}

void SetQuadRand(stQuad& Quad, float StartX, float StartY, float Limit, float Min)
{
	float randVal = rand() % ((int)Limit - (int)Min) + Min;
	Quad.p[0] = glm::vec2(StartX, StartY);
	Quad.p[1] = glm::vec2(StartX, StartY + randVal);
	Quad.p[2] = glm::vec2(StartX + randVal, StartY + randVal);
	Quad.p[3] = glm::vec2(StartX + randVal, StartY);
}

void QuadAddEdge(stQuad& Quad, float EdgeSize)
{
	Quad.p[0] = glm::vec2(Quad.p[0].x - EdgeSize, Quad.p[0].y - EdgeSize);
	Quad.p[1] = glm::vec2(Quad.p[1].x - EdgeSize, Quad.p[1].y + EdgeSize);
	Quad.p[2] = glm::vec2(Quad.p[2].x + EdgeSize, Quad.p[2].y + EdgeSize);
	Quad.p[3] = glm::vec2(Quad.p[3].x + EdgeSize, Quad.p[3].y - EdgeSize);
}

void QuadSize(stQuad& Quad, glm::vec2 Size)
{
	Quad.p[0].x -= Size.x;
	Quad.p[0].y -= Size.y;
	Quad.p[1].x -= Size.x;
	Quad.p[1].y += Size.y;
	Quad.p[2].x += Size.x;
	Quad.p[2].y += Size.y;
	Quad.p[3].x += Size.x;
	Quad.p[3].y -= Size.y;
}

stQuad QuadRotate(stQuad Quad, float Angle)
{
	glm::vec2 Center = GetQuadCenter2(Quad);
	MoveQuad(Quad, -Center);
	Quad.p[0] = glm::rotate(Quad.p[0], Angle);
	Quad.p[1] = glm::rotate(Quad.p[1], Angle);
	Quad.p[2] = glm::rotate(Quad.p[2], Angle);
	Quad.p[3] = glm::rotate(Quad.p[3], Angle);
	MoveQuad(Quad, Center);
	return Quad;
}

glm::vec2 GetQuadCenter(stQuad Quad)
{
	return glm::vec2((Quad.p[3].x - Quad.p[0].x) / 2 + Quad.p[0].x, (Quad.p[1].y - Quad.p[0].y) / 2 + Quad.p[0].y);
}

glm::vec2 GetQuadSize(stQuad Quad)
{
	return glm::vec2(Quad.p[3].x - Quad.p[0].x, Quad.p[1].y - Quad.p[0].y);
}

void MergeQuads(stQuad& Quad, stQuad Quad1, stQuad Quad2)
{
	glm::vec2 Min;
	glm::vec2 Max;
	if(Quad1.p[0].x < Quad2.p[0].x) Min.x = Quad1.p[0].x; else Min.x = Quad2.p[0].x;
	if(Quad1.p[0].y < Quad2.p[0].y) Min.y = Quad1.p[0].y; else Min.y = Quad2.p[0].y;
	if(Quad1.p[2].x > Quad2.p[2].x) Max.x = Quad1.p[2].x; else Max.x = Quad2.p[2].x;
	if(Quad1.p[2].y > Quad2.p[2].y) Max.y = Quad1.p[2].y; else Max.y = Quad2.p[2].y;
	SetQuadBy2Points(Quad, Min.x, Min.y, Max.x, Max.y);
}

glm::vec2 QuadMinPoint(stQuad Quad)
{
	glm::vec2 Min = Quad.p[0];
	Min.x = glm::min(Min.x, Quad.p[1].x);
	Min.x = glm::min(Min.x, Quad.p[2].x);
	Min.x = glm::min(Min.x, Quad.p[3].x);
	Min.y = glm::min(Min.y, Quad.p[1].y);
	Min.y = glm::min(Min.y, Quad.p[2].y);
	Min.y = glm::min(Min.y, Quad.p[3].y);
	return Min;
}

glm::vec2 QuadMaxPoint(stQuad Quad)
{
	glm::vec2 Max = Quad.p[0];
	Max.x = glm::max(Max.x, Quad.p[1].x);
	Max.x = glm::max(Max.x, Quad.p[2].x);
	Max.x = glm::max(Max.x, Quad.p[3].x);
	Max.y = glm::max(Max.y, Quad.p[1].y);
	Max.y = glm::max(Max.y, Quad.p[2].y);
	Max.y = glm::max(Max.y, Quad.p[3].y);
	return Max;
}

glm::vec2 GetQuadCenter2(stQuad Quad)
{
	glm::vec2 Min = QuadMinPoint(Quad);
	glm::vec2 Max = QuadMaxPoint(Quad);
	return glm::vec2((Min.x + Max.x) * 0.5f, (Min.y + Max.y) * 0.5f);
}

stQuad CreateQuad(float x0, float y0, float width, float height)
{
	stQuad RetQuad;
	SetQuad(RetQuad, x0, y0, width, height);
	return RetQuad;
}

void NullUVQuad(stUVQuad& Quad)
{
	Quad.p[0][1] = Quad.p[0][0] = glm::vec2(0, 0);
	Quad.p[1][1] = Quad.p[1][0] = glm::vec2(0, 0);
	Quad.p[2][1] = Quad.p[2][0] = glm::vec2(0, 0);
	Quad.p[3][1] = Quad.p[3][0] = glm::vec2(0, 0);
}

void SetUVQuad(stUVQuad& Quad, float PX0, float PY0, float PX1, float PY1, float PX2, float PY2, float PX3, float PY3,
	float TX0, float TY0, float TX1, float TY1, float TX2, float TY2, float TX3, float TY3)
{
	Quad.p[0][0] = glm::vec2(PX0, PY0);
	Quad.p[0][1] = glm::vec2(TX0, TY0);
	Quad.p[1][0] = glm::vec2(PX1, PY1);
	Quad.p[1][1] = glm::vec2(TX1, TY1);
	Quad.p[2][0] = glm::vec2(PX2, PY2);
	Quad.p[2][1] = glm::vec2(TX2, TY2);
	Quad.p[3][0] = glm::vec2(PX3, PY3);
	Quad.p[3][1] = glm::vec2(TX3, TY3);
}

void SetUVQuad(stUVQuad& Quad, float SVX, float SVY, float VW, float VH, float STX, float STY, float TW, float TH)
{
	Quad.p[0][0] = glm::vec2(SVX, SVY);
	Quad.p[1][0] = glm::vec2(SVX, SVY + VH);
	Quad.p[2][0] = glm::vec2(SVX + VW, SVY + VH);
	Quad.p[3][0] = glm::vec2(SVX + VW, SVY);
	Quad.p[0][1] = glm::vec2(STX, STY);
	Quad.p[1][1] = glm::vec2(STX, STY + TH);
	Quad.p[2][1] = glm::vec2(STX + TW, STY + TH);
	Quad.p[3][1] = glm::vec2(STX + TW, STY);
}

void SetUVQuad(stUVQuad& Quad, stQuad Vertex, stQuad UV)
{
	Quad.p[0][0] = Vertex.p[0];
	Quad.p[0][1] = UV.p[0];
	Quad.p[1][0] = Vertex.p[1];
	Quad.p[1][1] = UV.p[1];
	Quad.p[2][0] = Vertex.p[2];
	Quad.p[2][1] = UV.p[2];
	Quad.p[3][0] = Vertex.p[3];
	Quad.p[3][1] = UV.p[3];
}

void SetUVQuadVertex(stUVQuad& Quad, float SVX, float SVY, float VW, float VH)
{
	Quad.p[0][0] = glm::vec2(SVX, SVY);
	Quad.p[1][0] = glm::vec2(SVX, SVY + VH);
	Quad.p[2][0] = glm::vec2(SVX + VW, SVY + VH);
	Quad.p[3][0] = glm::vec2(SVX + VW, SVY);
}

void SetUVQuadVertexCH(stUVQuad& Quad, float CenterX, float CenterY, float HalfWidth, float HalfHeight)
{
	Quad.p[0][0] = glm::vec2(CenterX - HalfWidth, CenterY - HalfHeight);
	Quad.p[1][0] = glm::vec2(CenterX - HalfWidth, CenterY + HalfHeight);
	Quad.p[2][0] = glm::vec2(CenterX + HalfWidth, CenterY + HalfHeight);
	Quad.p[3][0] = glm::vec2(CenterX + HalfWidth, CenterY - HalfHeight);
}

void SetUVQuadTexture(stUVQuad& Quad, float STX, float STY, float TW, float TH)
{
	Quad.p[0][1] = glm::vec2(STX, STY);
	Quad.p[1][1] = glm::vec2(STX, STY + TH);
	Quad.p[2][1] = glm::vec2(STX + TW, STY + TH);
	Quad.p[3][1] = glm::vec2(STX + TW, STY);
}

void MoveUVQuad(stUVQuad& UVQuad, float X, float Y)
{
	UVQuad.p[0][0] += glm::vec2(X, Y);
	UVQuad.p[1][0] += glm::vec2(X, Y);
	UVQuad.p[2][0] += glm::vec2(X, Y);
	UVQuad.p[3][0] += glm::vec2(X, Y);
}

void MoveUVQuad(stUVQuad& UVQuad, glm::vec2 Velocity)
{
	UVQuad.p[0][0] += Velocity;
	UVQuad.p[1][0] += Velocity;
	UVQuad.p[2][0] += Velocity;
	UVQuad.p[3][0] += Velocity;
}

stQuad GetVertexQuad(stUVQuad UVQuad)
{
	stQuad Quad;
	
	Quad.p[0] = UVQuad.p[0][0];
	Quad.p[1] = UVQuad.p[1][0];
	Quad.p[2] = UVQuad.p[2][0];
	Quad.p[3] = UVQuad.p[3][0];
	
	return Quad;
}

stQuad GetUVQuad(stUVQuad UVQuad)
{
	stQuad Quad;
	
	Quad.p[0] = UVQuad.p[0][1];
	Quad.p[1] = UVQuad.p[1][1];
	Quad.p[2] = UVQuad.p[2][1];
	Quad.p[3] = UVQuad.p[3][1];
	
	return Quad;
}

bool QuadQuadCollision(stQuad Quad1, stQuad Quad2)
{
	return !(Quad1.p[0].x < Quad2.p[0].x && Quad1.p[2].x < Quad2.p[0].x ||
		Quad1.p[0].x > Quad2.p[2].x && Quad1.p[2].x > Quad2.p[2].x ||
		Quad1.p[0].y < Quad2.p[0].y && Quad1.p[2].y < Quad2.p[0].y ||
		Quad1.p[0].y > Quad2.p[2].y && Quad1.p[2].y > Quad2.p[2].y);
}

bool QuadRayIntersect(stQuad Quad, glm::vec2 CurrentPoint, glm::vec2 Velocity, glm::vec2& IntersectPoint)
{
	//no velocity no collisions
	if(!Velocity.x && !Velocity.y) return false;

	//point alredy in quad
	if((CurrentPoint.x > Quad.p[0].x && CurrentPoint.x < Quad.p[2].x) && (CurrentPoint.y > Quad.p[0].y && CurrentPoint.y < Quad.p[2].y)) return true;
	
	std::vector<glm::vec2> IntersectPoints;
	glm::vec2 Near, Far;
	glm::vec2 NextPoint = CurrentPoint + Velocity;
	float MinDistance;
	IntersectPoint = glm::vec2(0, 0);
	
	//calculate near far collision points (0 - x, 1 - y)
	for(int i=0; i<2; i++)
	{
		if(Velocity[i] != 0)
		{
			Near[i] = Quad.p[0][i];
			Near[1-i] = (NextPoint[1-i] - CurrentPoint[1-i]) * (Near[i] - CurrentPoint[i]) / (NextPoint[i] - CurrentPoint[i]) + CurrentPoint[1-i];
			Far[i] = Quad.p[2][i];
			Far[1-i] = (NextPoint[1-i] - CurrentPoint[1-i]) * (Far[i] - CurrentPoint[i]) / (NextPoint[i] - CurrentPoint[i]) + CurrentPoint[1-i];
			if((Near.x >= Quad.p[0].x && Near.x <= Quad.p[2].x) && (Near.y >= Quad.p[0].y && Near.y <= Quad.p[2].y)) IntersectPoints.push_back(Near);
			if((Far.x >= Quad.p[0].x && Far.x <= Quad.p[2].x) && (Far.y >= Quad.p[0].y && Far.y <= Quad.p[2].y)) IntersectPoints.push_back(Far);
		}
	}
	
	//test that intersect points exists
	if(!IntersectPoints.size()) return false;
	
	//find nearest collision point
	MinDistance = glm::distance(CurrentPoint, IntersectPoints[0]);
	for(int i=0; i<IntersectPoints.size(); i++)
	{
		if(MinDistance > glm::distance(CurrentPoint, IntersectPoints[i]))
		{
			MinDistance = glm::distance(CurrentPoint, IntersectPoints[i]);
			IntersectPoint = IntersectPoints[i];
		}
	}
	//std::cout<<"i: "<<IntersectPoint.x<<" "<<IntersectPoint.y<<" c: "<<CurrentPoint.x<<" "<<CurrentPoint.y<<" n: "<<NextPoint.x<<" "<<NextPoint.y<<
	//	" md: "<<MinDistance<<" d: "<<glm::distance(CurrentPoint, NextPoint) + 0.02f<<" bo: "<<(MinDistance <= glm::distance(CurrentPoint, NextPoint) + 0.02f)<<std::endl;
	
	IntersectPoints.clear();
	return MinDistance <= glm::distance(CurrentPoint, NextPoint) + 0.02f;
}

bool QuadQuadIntersect(stQuad StaticQuad, stQuad DynamicQuad, glm::vec2 Velocity, glm::vec2& IntersectPoint)
{
	glm::vec2 DynamicQuadHalfSize = glm::vec2((DynamicQuad.p[2].x - DynamicQuad.p[0].x) * 0.5f, (DynamicQuad.p[2].y - DynamicQuad.p[0].y) * 0.5f);
	glm::vec2 DynamicQuadCenter = glm::vec2((DynamicQuad.p[2].x + DynamicQuad.p[0].x) * 0.5f, (DynamicQuad.p[2].y + DynamicQuad.p[0].y) * 0.5f);
	QuadSize(StaticQuad, DynamicQuadHalfSize);
	
	return QuadRayIntersect(StaticQuad, DynamicQuadCenter, Velocity, IntersectPoint);
}
