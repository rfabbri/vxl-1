function [UTME, UTMN]  = lltoutm(Lat, Long);

% #define        
WGS84_A =                6378137.0;
% #define   
WGS84_ECCSQ =            0.00669437999013;
M_PI = pi;


%--------------------------------------------------   
% void dgc_LLtoUTM(double Lat, double Long, double *UTMNorthing, 
%                  double *UTMEasting, char *UTMZone)

k0 = 0.9996; % , N, T, C, A, M;
LatRad = Lat * M_PI / 180.0;
LongRad = Long * M_PI / 180.0;
  

ZoneNumber = floor((Long + 180.0)/6) + 1;
  
if(Lat >= 56.0 && Lat < 64.0 && Long >= 3.0 && Long < 12.0)
    ZoneNumber = 32;
end;

% Special zones for Svalbard
if(Lat >= 72.0 && Lat < 84.0) 
   if(Long >= 0.0  && Long <  9.0) 
          ZoneNumber = 31;
      elseif(Long >= 9.0  && Long < 21.0) 
          ZoneNumber = 33;
      elseif(Long >= 21.0 && Long < 33.0) 
          ZoneNumber = 35;
      elseif(Long >= 33.0 && Long < 42.0) 
          ZoneNumber = 37;
      end;
end;

        
% +3 puts origin in middle of zone
LongOrigin = double((ZoneNumber - 1) * 6 - 180.0 + 3);  
LongOriginRad = LongOrigin * M_PI / 180.0;

% compute the UTM Zone from the latitude and longitude
% UTMZone = sprintf('%d%c', ZoneNumber, UTMLetterDesignator(Lat));
  
 eccPrimeSquared = WGS84_ECCSQ / (1 - WGS84_ECCSQ);
 N = WGS84_A / sqrt(1 - WGS84_ECCSQ * sin(LatRad) * sin(LatRad));
 T = tan(LatRad) * tan(LatRad);
 C = eccPrimeSquared * cos(LatRad) * cos(LatRad);
 A = cos(LatRad) * double(LongRad-LongOriginRad);
 M = WGS84_A * ((1 - WGS84_ECCSQ / 4 - 3 * WGS84_ECCSQ * WGS84_ECCSQ / 64 ...
            - 5 * WGS84_ECCSQ * WGS84_ECCSQ * WGS84_ECCSQ / 256) * LatRad ...
           - (3 * WGS84_ECCSQ / 8 + 3 * WGS84_ECCSQ * WGS84_ECCSQ / 32 ...
              + 45 * WGS84_ECCSQ * WGS84_ECCSQ * WGS84_ECCSQ / 1024) * ...
           sin(2 * LatRad) + (15 * WGS84_ECCSQ * WGS84_ECCSQ / 256 + ...
                              45 * WGS84_ECCSQ * WGS84_ECCSQ * ...
                              WGS84_ECCSQ / 1024) * sin(4 * LatRad) ... 
           - (35 * WGS84_ECCSQ * WGS84_ECCSQ * WGS84_ECCSQ / 3072) * ...
           sin(6 * LatRad));
 UTMEasting = double(k0 * N * (A + (1 - T + C) * A * A * A / 6 ...
                                   + (5 - 18 * T + T * T + 72 * C - ...
                                      58 * eccPrimeSquared)* ...
                                  A * A * A * A *A / 120) + 500000.0);
 UTMNorthing = double(k0 * (M + N * tan(LatRad) * ...
                                (A * A / 2 + (5 - T + 9 * C + 4 * C * C) ...
                                 * A * A * A *A / 24 ...
                                 + (61 - 58 * T + T * T + ... 
                                    600 * C - 330 * eccPrimeSquared) * ... 
                                 A * A * A * A * A * A / 720))); 
  if(Lat < 0)
    UTMNorthing = UTMNorthing + 10000000.0;  % 10000000 meter offset for southern hemisphere  
  end
  
  UTME = UTMEasting;
  UTMN = UTMNorthing;
  
  
  
function [LetterDesignator] = UTMLetterDesignator(Lat)

if((84 >= Lat) && (Lat >= 72)) LetterDesignator = 'X';
elseif((72 > Lat) && (Lat >= 64)) LetterDesignator = 'W';
elseif((64 > Lat) && (Lat >= 56)) LetterDesignator = 'V';
elseif((56 > Lat) && (Lat >= 48)) LetterDesignator = 'U';
elseif((48 > Lat) && (Lat >= 40)) LetterDesignator = 'T';
elseif((40 > Lat) && (Lat >= 32)) LetterDesignator = 'S';
elseif((32 > Lat) && (Lat >= 24)) LetterDesignator = 'R';
elseif((24 > Lat) && (Lat >= 16)) LetterDesignator = 'Q';
elseif((16 > Lat) && (Lat >= 8)) LetterDesignator = 'P';
elseif(( 8 > Lat) && (Lat >= 0)) LetterDesignator = 'N';
elseif(( 0 > Lat) && (Lat >= -8)) LetterDesignator = 'M';
elseif((-8> Lat) && (Lat >= -16)) LetterDesignator = 'L';
elseif((-16 > Lat) && (Lat >= -24)) LetterDesignator = 'K';
elseif((-24 > Lat) && (Lat >= -32)) LetterDesignator = 'J';
elseif((-32 > Lat) && (Lat >= -40)) LetterDesignator = 'H';
elseif((-40 > Lat) && (Lat >= -48)) LetterDesignator = 'G';
elseif((-48 > Lat) && (Lat >= -56)) LetterDesignator = 'F';
elseif((-56 > Lat) && (Lat >= -64)) LetterDesignator = 'E';
elseif((-64 > Lat) && (Lat >= -72)) LetterDesignator = 'D';
elseif((-72 > Lat) && (Lat >= -80)) LetterDesignator = 'C';
else LetterDesignator = 'Z';
end;

return;

    