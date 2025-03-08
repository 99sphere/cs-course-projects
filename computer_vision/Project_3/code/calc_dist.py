import numpy as np
import math

def calc_side_length(pts):
    def dist(x, y):
        return math.sqrt((x[0]-y[0])**2 + (x[1]-y[1])**2 + (x[2]-y[2])**2)
    
    left_length = dist(pts[2], pts[0])
    right_length = dist(pts[3], pts[1])
    top_length = dist(pts[2], pts[3])
    bottom_length = dist(pts[1], pts[0])

    print(f"left: {left_length}, right: {right_length}, top: {top_length}, bottom: {bottom_length}")
    return


if __name__ == "__main__":
    
    case_1 = [
        [-231.19, 81.99, 620.25],
        [-34.69, 88.16, 585.5 ],
        [-230.12, -22.32, 637.8 ],
        [-31.16, -5.81, 607.46]    ,
    ]


    case_2 = [
        [-216.39, 58.85, 620.2 ],
        [-20.73, 71.12, 582.68],
        [-211.43, -35.93, 638.9 ],
        [-13.16, -22.29, 602.8 ],
    ]
    case_3 = [        
        [-94.3, 75.47, 702.37],
        [ 94.68, 88.86, 645.32],
        [-82.49, -15.37, 740.09],
        [108.88, 1.86, 682.78],
    ]

    case_4 = [
        [-81.95, 77.35, 559.8 ],
        [109.97, 86.5, 568.01],
        [-80.26, -17.57, 591.4 ],
        [113.78, -3.92, 605.45],
    ]
    
    case_5 = [
        [-128.78, 59.19, 703.01],
        [ 67.27, 66.58, 718.43],
        [-127.66, -32.58, 735.89],
        [ 62.35, -26.28, 756.26],
    ]
    
    calc_side_length(case_1)
    calc_side_length(case_2)
    calc_side_length(case_3)
    calc_side_length(case_4)
    calc_side_length(case_5)