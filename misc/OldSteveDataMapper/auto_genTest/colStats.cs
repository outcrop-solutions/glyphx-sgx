using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace IngestionEngine
{
    class colStats : INotifyPropertyChanged
    {
        private string      _colName;
        private double      _colMin;
        private double      _colMax;
        private double      _colAvg;
        private int         _colNum;
        private double      _colMaxout;
        private double      _colMinout;
        private double      _colClipMaxout;
        private double      _colClipMinout;
        private int         _antLevel;
        private bool        _antOut;
        private string      _antParm;
        //public DataSet dSet = new DataSet();


        public event PropertyChangedEventHandler PropertyChanged;
        public decimal nfooo { get; set; }
        public string colName
        { 
            get { return _colName; }
            set {
                _colName = value;
                this.NotifyPropertyChanged("colName");
            }
        }
        public double colMin
        {
            get { return _colMin; }
            set
            {
                _colMin = value;
                this.NotifyPropertyChanged("colMin");
            }
        }
        public double colMax
        {
            get { return _colMax; }
            set
            {
                _colMax = value;
                this.NotifyPropertyChanged("colMax");
            }
        }
        public double colAvg
        {
            get { return _colAvg; }
            set
            {
                _colAvg = value;
                this.NotifyPropertyChanged("colAvg");
            }
        }
        public double colNum
        {
            get { return _colNum; }
            set
            {
                _colNum = Convert.ToInt32(value);
                this.NotifyPropertyChanged("colNum");
            }
        }

        public double colMaxout
        {
            get { return _colMaxout; }
            set
            {
                _colMaxout = value;
                this.NotifyPropertyChanged("colMaxout");
            }
        }
        public double colMinout
        {
            get { return _colMinout; }
            set
            {
                _colMinout = value;
                this.NotifyPropertyChanged("colMinout");
            }
        }
        public double colClipMaxout
        {
            get { return _colClipMaxout; }
            set
            {
                _colClipMaxout = value;
                this.NotifyPropertyChanged("colClipMaxout");
            }
        }
        public double colClipMinout
        {
            get { return _colClipMinout; }
            set
            {
                _colClipMinout = value;
                this.NotifyPropertyChanged("colClipMin  ");
            }
        }
        public int antLevel
        {
            get { return _antLevel; }
            set
            {
                _antLevel = value;
                this.NotifyPropertyChanged("antLevel");
            }
        }
        public bool antOut
        {
            get { return _antOut; }
            set
            {
                _antOut = value;
                this.NotifyPropertyChanged("antOut");
            }
        }
        public string antParm
        {
            get { return _antParm; }
            set
            {
                _antParm = value;
                this.NotifyPropertyChanged("antParm");
            }
        }
        private void NotifyPropertyChanged(string name)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(name));
        }

        public string toString()
        {
            return "colName = " + _colName + ", _colMax" + _colMax + ", _colMin" + _colMin + ", _colAvg" + _colAvg + ", _colNum" + _colNum;
        }
    }
}
