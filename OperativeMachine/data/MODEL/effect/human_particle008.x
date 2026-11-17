xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 21;
 -7.76502;6.00000;15.00000;,
 -7.76502;3.00000;14.19615;,
 2.27316;3.00000;10.03818;,
 2.84157;6.00000;10.60659;,
 6.43113;3.00000;0.00000;,
 7.23498;6.00000;0.00000;,
 2.27316;3.00000;-10.03818;,
 2.84157;6.00000;-10.60659;,
 -7.76502;3.00000;-14.19615;,
 -7.76502;6.00000;-15.00000;,
 -7.76502;0.00000;9.00000;,
 -7.76502;0.00000;0.00000;,
 -1.40106;0.00000;6.36396;,
 1.23498;0.00000;0.00000;,
 -1.40106;0.00000;-6.36396;,
 -7.76502;0.00000;-9.00000;,
 2.84157;32.96211;10.60659;,
 -7.76502;33.77496;15.00000;,
 7.23498;28.74735;0.00000;,
 2.84157;31.37436;-10.60659;,
 -7.76502;39.35775;-15.00000;;
 
 21;
 4;0,1,2,3;,
 4;3,2,4,5;,
 4;5,4,6,7;,
 4;7,6,8,9;,
 3;10,11,12;,
 3;12,11,13;,
 3;13,11,14;,
 3;14,11,15;,
 4;0,9,8,1;,
 4;16,17,0,3;,
 4;18,16,3,5;,
 4;19,18,5,7;,
 4;20,19,7,9;,
 4;17,20,9,0;,
 4;16,19,20,17;,
 3;16,18,19;,
 4;2,1,10,12;,
 4;4,2,12,13;,
 4;6,4,13,14;,
 4;8,6,14,15;,
 4;1,8,15,10;;
 
 MeshMaterialList {
  4;
  21;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  3,
  0,
  0,
  0,
  0,
  3,
  3,
  3,
  0,
  0,
  0,
  0,
  3;;
  Material {
   0.800000;0.517647;0.000000;1.000000;;
   5.000000;
   0.260000;0.260000;0.260000;;
   0.000000;0.000000;0.000000;;
  }
  Material {
   0.765600;0.596000;0.486400;1.000000;;
   13.000000;
   0.260000;0.260000;0.260000;;
   0.086130;0.067050;0.054720;;
   TextureFilename {
    "C:\\Users\\student\\source\\repos\\DirectX2026_100\\data\\TEXTURE\\white.png";
   }
  }
  Material {
   0.765600;0.596000;0.486400;1.000000;;
   13.000000;
   0.260000;0.260000;0.260000;;
   0.086130;0.067050;0.054720;;
   TextureFilename {
    "C:\\Users\\student\\source\\repos\\DirectX2026_100\\data\\TEXTURE\\face001.png";
   }
  }
  Material {
   0.656000;0.000000;0.081600;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
 MeshNormals {
  24;
  0.379871;-0.121040;-0.917086;,
  0.379871;-0.121040;0.917086;,
  0.701027;-0.130848;0.701027;,
  0.991402;-0.130849;0.000000;,
  0.701027;-0.130848;-0.701027;,
  0.309790;-0.587094;-0.747897;,
  0.309790;-0.587094;0.747897;,
  0.556206;-0.617470;0.556206;,
  0.786594;-0.617471;-0.000000;,
  0.556206;-0.617470;-0.556206;,
  0.105489;-0.961257;-0.254672;,
  0.105489;-0.961257;0.254672;,
  0.181091;-0.966650;0.181091;,
  0.256101;-0.966650;-0.000000;,
  0.181091;-0.966650;-0.181091;,
  0.000000;-1.000000;-0.000000;,
  0.382685;0.000000;-0.923879;,
  0.382685;0.000000;0.923879;,
  0.707107;0.000000;0.707107;,
  0.952164;0.304736;-0.022809;,
  0.707107;0.000000;-0.707107;,
  -1.000000;0.000000;0.000000;,
  0.510780;0.859708;0.002323;,
  0.398325;0.915040;0.063554;;
  21;
  4;1,6,7,2;,
  4;2,7,8,3;,
  4;3,8,9,4;,
  4;4,9,5,0;,
  3;11,15,12;,
  3;12,15,13;,
  3;13,15,14;,
  3;14,15,10;,
  4;21,21,21,21;,
  4;18,17,1,2;,
  4;19,18,2,3;,
  4;20,19,3,4;,
  4;16,20,4,0;,
  4;21,21,21,21;,
  4;22,22,23,23;,
  3;22,19,22;,
  4;7,6,11,12;,
  4;8,7,12,13;,
  4;9,8,13,14;,
  4;5,9,14,10;,
  4;21,21,21,21;;
 }
 MeshTextureCoords {
  21;
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;;
 }
}
