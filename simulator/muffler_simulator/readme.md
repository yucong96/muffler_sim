## Usage
```
mkdir build
cd build
cmake ..
make
cd bin
./muffler {model-name}
```
Here, {model-name} can be "tiny".

## Structure
* /src

	main code

* /config

	set the parameter like frequency, air density, sound speed, inlet & outlet plane function, applied sound pressure at inlet (which can be a complex number)

* /tet

	input vtk model

* /output

	output model with sound pressure field

* /tools

	geo_gen can transfer length, radius to generate .geo file

	netgen can generate the mesh of .geo file and get a .vol result

	vol2vtk can transfer .vol file to .vtk file

* /log

	some logs

## Governing Equation and Boundary Condition

The governing equation of acoustic sound is a Helmholzs equation:
\bigtriangleup p - \frac{\omega^2}{c^2} p = 0
<img src="https://latex.codecogs.com/gif.latex?\bigtriangleup p - \frac{\omega^2}{c^2} p = 0"/>

where 
<img src="https://latex.codecogs.com/gif.latex?p"/> denotes the sound pressure,
<img src="https://latex.codecogs.com/gif.latex?\bigtriangleup"/> denotes the laplacian operator,
<img src="https://latex.codecogs.com/gif.latex?\omega"/> denotes the angular frequency,
<img src="https://latex.codecogs.com/gif.latex?c"/> denotes the sound speed in the particular media.

There are three kinds of boundary conditions. On the hard wall, the boundary condition is
 <img src="https://latex.codecogs.com/gif.latex?\bigtriangledown"/>
 <img src="https://latex.codecogs.com/gif.latex?p"/>
 <img src="https://latex.codecogs.com/gif.latex?\cdot"/>
 <img src="https://latex.codecogs.com/gif.latex?n"/>
 <img src="https://latex.codecogs.com/gif.latex?="/>
 <img src="https://latex.codecogs.com/gif.latex?0"/>

where
<img src="https://latex.codecogs.com/gif.latex?\bigtriangledown"/> denotes the gradient,
<img src="https://latex.codecogs.com/gif.latex?n"/> denotes the normal direction of the wall.

At the outlet of muffler, we only consider an outcoming wave, so the boundary condition is
\bigtriangledown p \cdot n + \frac{i \omega}{c}p = 0
<img src="https://latex.codecogs.com/gif.latex?\bigtriangledown p \cdot n + \frac{i \omega}{c}p = 0"/>

where
<img src="https://latex.codecogs.com/gif.latex?i"/> is the imagarinary unit.

At the inlet of muffler, the sound wave is a combination of incoming wave and outcoming wave, so the boundary condition is
\bigtriangledown p \cdot n + \frac{i \omega}{c}p - \frac{2 i \omega}{c}p_0 = 0
<img src="https://latex.codecogs.com/gif.latex?\bigtriangledown p \cdot n + \frac{i \omega}{c}p - \frac{2 i \omega}{c}p_0 = 0"/>

where
<img src="https://latex.codecogs.com/gif.latex?p_0"/> is the applied sound pressure.