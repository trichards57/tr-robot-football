clear all
close all

resolution = 0.5;
fieldWidth = 220;
fieldHeight = 180;

attractField = zeros(fieldWidth/resolution,fieldHeight/resolution);
repelField = zeros(fieldWidth/resolution,fieldHeight/resolution);
speedX = zeros(fieldWidth/resolution,fieldHeight/resolution);
speedY = zeros(fieldWidth/resolution,fieldHeight/resolution);

ball = BasicObject;
ball.X = 110;
ball.Y = 90;
ball.Weight = 150;

opponentsWeight = 1;
opponentsEffectRadius = 100;

opponents(5) = BasicObject;

opponents(1).X = 55;
opponents(1).Y = 50;
opponents(1).Weight = opponentsWeight;
opponents(2).X = (fieldWidth-55);
opponents(2).Y = 50;
opponents(2).Weight = opponentsWeight;
opponents(3).X = (fieldWidth-55);
opponents(3).Y = (fieldHeight-50);
opponents(3).Weight = opponentsWeight;
opponents(4).X = 55;
opponents(4).Y = (fieldHeight-50);
opponents(4).Weight = opponentsWeight;

opponents(5).X = (fieldWidth-15);
opponents(5).Y = fieldHeight/2;
opponents(5).Weight = opponentsWeight;

for x= 1:resolution:fieldWidth
    for y = 1:resolution:fieldHeight
        i = x / resolution;
        j = y / resolution;
        
        xDiff = ball.X - x;
        yDiff = ball.Y - y;
        dist = sqrt(xDiff^2 + yDiff^2);
        attractField(i,j) = 1/2 * ball.Weight * dist^2;
        
        for k=1:size(opponents,2)
            xDiff = opponents(k).X - x;
            yDiff = opponents(k).Y - y;
            dist = sqrt(xDiff^2 + yDiff^2) - 11; % 11 cm to allow for width of the robot
            
            dist = max(dist, 0.001);
            
            %if (dist < opponentsEffectRadius)
                we = (1/2 * opponents(k).Weight * (1/dist - 1/opponentsEffectRadius)^2);
                repelField(i,j) = repelField(i,j) + we;
            %end
        end
    end
end

field = attractField + repelField;

[px,py] = gradient(field);

for x= 1:resolution:fieldWidth
    for y = 1:resolution:fieldHeight
        i = x / resolution;
        j = y / resolution;
        
        xDiff = ball.X - x;
        yDiff = ball.Y - y;
        
        angle = atan(py(i,j)/px(i,j));
        
        if (px(i,j) ~= abs(px(i,j)))
            angle = angle + pi;
        end
        
        angleToTarget = atan(yDiff/xDiff);
        if (xDiff ~= abs(xDiff))
            angleToTarget = angleToTarget + pi;
        end
        motionAngle = angle-angleToTarget;
        
        if repelField(i,j)==0
            speedX(i,j) = 100 * cos(angle);
            speedY(i,j) = 100*sin(angle);
        else
            speedX(i,j) = 100*(1-abs(cos(motionAngle)))*cos(angle);
            speedY(i,j) = 100*(1-abs(cos(motionAngle)))*sin(angle);
        end
    end
end

surf(field', 'EdgeColor', 'None');
figure;
[px,py] = gradient(field');
contour(field'), hold on, quiver(-px,-py), hold off
figure;
quiver(speedY', speedX');