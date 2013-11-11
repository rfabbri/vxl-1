function varargout = slice_selector(varargin)
% SLICE_SELECTOR MATLAB code for slice_selector.fig
%      SLICE_SELECTOR, by itself, creates a new SLICE_SELECTOR or raises the existing
%      singleton*.
%
%      H = SLICE_SELECTOR returns the handle to a new SLICE_SELECTOR or the handle to
%      the existing singleton*.
%
%      SLICE_SELECTOR('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SLICE_SELECTOR.M with the given input arguments.
%
%      SLICE_SELECTOR('Property','Value',...) creates a new SLICE_SELECTOR or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before slice_selector_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to slice_selector_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help slice_selector

% Last Modified by GUIDE v2.5 10-Apr-2012 17:30:23

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @slice_selector_OpeningFcn, ...
                   'gui_OutputFcn',  @slice_selector_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT



% --- Executes just before slice_selector is made visible.
function slice_selector_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to slice_selector (see VARARGIN)

% Choose default command line output for slice_selector
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);
global V pos; 
imagesc(V(:,:,1), [0 255]);
colormap gray;
axis image;
axis off;
pos = 1;

% UIWAIT makes slice_selector wait for user response (see UIRESUME)
uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = slice_selector_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
%varargout{1} = handles.output;


% --- Executes on slider movement.
function slider1_Callback(hObject, eventdata, handles)
% hObject    handle to slider1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global V pos;
pos = uint16(get(hObject,'Value'));
imagesc(V(:,:,pos), [0 255]);
colormap gray;
axis image;
axis off;


% --- Executes during object creation, after setting all properties.
function slider1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to slider1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
global V;
maxval = size(V, 3);
set(hObject,'Min',1);
set(hObject,'Max',maxval);
set(hObject,'Value',1);
set(hObject,'SliderStep',[1/maxval 10/maxval]);


% --- Executes on button press in firstslice.
function firstslice_Callback(hObject, eventdata, handles)
% hObject    handle to firstslice (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global pos start_index;
start_index = pos

% --- Executes on button press in lastslice.
function lastslice_Callback(hObject, eventdata, handles)
% hObject    handle to lastslice (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global pos end_index;
end_index = pos


% --- Executes on button press in continuebutton.
function continuebutton_Callback(hObject, eventdata, handles)
% hObject    handle to continuebutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global start_index end_index
if start_index > end_index
	t = start_index;
	start_index = end_index;
	end_index = t;
end
uiresume(handles.figure1)
delete(handles.figure1)
