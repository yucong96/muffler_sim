#! python2
import sys
from paraview.simple import *

def load_geo(input_file):
    geometry = LegacyVTKReader(FileNames=[input_file])
    return geometry

def extract_surf(geometry):
    surface = ExtractSurface(Input=geometry)
    return surface

def save_vtk(vtk_file, surface):
    SaveData(vtk_file, proxy=surface, FileType='Ascii')

def clip_geo(geometry):
    clip = Clip(Input=geometry)
    #clip.ClipType = 'Plane'
    #clip.Scalars = ['POINTS', 'point_scalar_data']
    #clip.Value = 0.7126249969005585
    #clip.ClipType.Origin = [0.0, 0.15000000596046448, 0.0]
    return clip

def coutour_geo(geometry):
    contour = Contour(Input=clip)
    #contour.ContourBy = ['POINTS', 'point_scalar_data']
    #contour.Isosurfaces = [0.7126249969005585]
    #contour.PointMergeMethod = 'Uniform Binning'
    contour.Isosurfaces = [0.31254, 0.369695, 0.42684999999999995, 0.484005, 0.54116, 0.5983149999999999, 0.65547, 0.7126250000000001, 0.7697799999999999, 0.8269350000000001, 0.88409, 0.9412450000000001, 0.9984000000000001, 1.0555550000000002, 1.11271]
    return contour


def extract_pure_surf(input_file, output_file):
    geometry = load_geo(input_file)
    surface = extract_surf(geometry)
    save_vtk(output_file, surface)

def extract_clip_surf(input_file, output_file):
    geometry = load_geo(input_file)
    clip = clip_geo(geometry)
    surface = extract_surf(clip)
    save_vtk(output_file, surface)

def extract_coutour_surf(input_file, output_file):
    geometry = load_geo(input_file)
    clip = clip_geo(geometry)
    coutour = coutour_geo(coutour)
    surface = extract_surf(coutour)
    save_vtk(output_file, surface)

    
if __name__ == '__main__':
    if len(sys.argv) == 4:
        command_type = sys.argv[1]
        input_file = sys.argv[2]
        output_file = sys.argv[3]
        if command_type == 'extract_pure_surf':
            extract_pure_surf(input_file, output_file)
        elif command_type == 'extract_clip_surf':
            extract_clip_surf(input_file, output_file)
        elif command_type == 'extract_coutour_surf':
            extract_coutour_surf(input_file, output_file)
        else:
            print('command error')
    else:
        print('format error')
