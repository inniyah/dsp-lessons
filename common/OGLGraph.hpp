/*
    Implementation of the OGLGraph class functions
    Copyright (C) 2006 Gabriyel Wong (gabriyel@gmail.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "OGLGraph.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>

template<unsigned int CHANNELS>
void OGLGraph<CHANNELS>::setup(int cache_size, double min_value, double max_value) {

	if (max_value <= min_value) {
		double tmp = max_value;
		max_value = min_value + 0.1;
		min_value = tmp - 0.1;
	}

	_maxValue = max_value;
	_minValue = min_value;
	_maxDetectedValue = 0.0;
	_minDetectedValue = 0.0;
	_cacheSize = cache_size;
}

template<unsigned int CHANNELS>
void OGLGraph<CHANNELS>::update(const double data[CHANNELS]) {

	double k_forget = 4.0 * _cacheSize;
	double alpha = 1 / k_forget;
	double max = _maxDetectedValue;
	double min = _minDetectedValue;
	_maxDetectedValue = alpha * min + (1.0 - alpha) * max;
	_minDetectedValue = alpha * max + (1.0 - alpha) * min;

	for (unsigned int channel = 0; channel < CHANNELS; ++channel) {
		if ( _data[channel].size() > (unsigned int)(_cacheSize) ) { 
			_data[channel].pop_front();
			_data[channel].push_back( data[channel] );
		} else {
			_data[channel].push_back( data[channel] );
		}

		for (std::list<float>::const_iterator iter = _data[channel].begin(); iter != _data[channel].end(); iter++ ) {
			if ((*iter) > _maxDetectedValue) {
				_maxDetectedValue = (*iter);
			}
			if ((*iter) < _minDetectedValue) {
				_minDetectedValue = (*iter);
			}
		}
	}
}

template<unsigned int CHANNELS>
void OGLGraph<CHANNELS>::draw() {
	int cnt;

	// Set up the display
	glMatrixMode(GL_PROJECTION); // We'll talk about this one more as we go 
	glLoadIdentity();

	double k_adjust = _cacheSize/4.0;
	double alpha = 1 / k_adjust;
	double margin = (_maxDetectedValue - _minDetectedValue) * 0.05;
	_maxValue = alpha * (_maxDetectedValue + margin) + (1.0 - alpha) * _maxValue;
	_minValue = alpha * (_minDetectedValue - margin) + (1.0 - alpha) * _minValue;

	float screen_w = 1.0;
	float screen_h = 1.0;
	float margin_w = 0.02 * screen_w;
	float margin_h = 0.02 * screen_h;
	float data_max = _maxValue;
	float data_min = _minValue;

	if (data_max <= data_min) {
		return;
	}

	glOrtho(0, screen_w, 0, screen_h, 0, 1.0); 

	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();

	// Draw a reference line in zero
	glEnable(GL_LINE_STIPPLE);
	glLineStipple (3, 0xAAAA);
	glBegin(GL_LINES);
	double linc = pow(10, floor(log10( (data_max - data_min)*0.8 )));
	for (double i = ceil(data_min/linc)*linc; i <= floor(data_max/linc)*linc; i += linc) {
		if (((-0.1*linc) < i) && (i < (0.1 * linc))) {
			glColor3ub(192, 192, 192);
		} else {
			glColor3ub(128, 128, 128);
		}
		float y = margin_h + (screen_h-2*margin_h) * (i-data_min) / (data_max-data_min);
		glVertex3f( margin_w,          y, 0 );
		glVertex3f( screen_w-margin_w, y, 0 );
	}
	glEnd();
	// Draw the axes - OpenGL screen coordinates start from bottom-left (0,0)
	glDisable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
	glColor3ub(255, 255, 255);
	// Draw x-axis
	glVertex3f( margin_w,          margin_h, 0 );
	glVertex3f( screen_w-margin_w, margin_h, 0 );
	// Draw y-axis
	glVertex3f( margin_w, margin_h,          0 );
	glVertex3f( margin_w, screen_h-margin_h, 0 );
	glEnd();

	for (unsigned int channel = 0; channel < CHANNELS; ++channel) {

		if (0 < LineConfig[channel].StippleFactor) {
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(LineConfig[channel].StippleFactor, LineConfig[channel].StipplePattern);
		} else {
			glDisable(GL_LINE_STIPPLE);
		}

		// Draw the data points
		glBegin(GL_LINE_STRIP);
		glColor3ub(LineConfig[channel].Red, LineConfig[channel].Green, LineConfig[channel].Blue);
		cnt = 0;
		for (std::list<float>::const_iterator iter = _data[channel].begin(); iter != _data[channel].end(); iter++ ) {
			double x = margin_w + (screen_w-2*margin_w) * cnt / _cacheSize;
			double y = margin_h + (screen_h-2*margin_h) * ((*iter)-data_min) / (data_max-data_min);
			y = fmax(0.0, fmin(screen_h, y));
			glVertex3f(x, y, 0);
			cnt++;
		}
		glEnd();

	}
}
