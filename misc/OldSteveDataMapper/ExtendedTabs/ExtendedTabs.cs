using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ExtendedTabs
{
    public partial class ExtendedTabControl : TabControl
    {
        List<TabPage> AllTabPages;

        public ExtendedTabControl()
        {
            AllTabPages = new List<TabPage>();
        }
        public void HideTabPage(TabPage tb)
        {
            if (TabPages.Contains(tb))
            {

                foreach (TabPage t in TabPages)
                {
                    if (!AllTabPages.Contains(t))
                        AllTabPages.Add(t);
                }
                this.TabPages.Remove(tb);
            }
        }

        public void ShowTabPage(TabPage tb)
        {
            if ((AllTabPages.Contains(tb)) && (!TabPages.Contains(tb)))
                this.TabPages.Add(tb);

        }
        protected override void OnSelecting(TabControlCancelEventArgs e)
        {
            if (e.TabPageIndex > -1)
            {
                TabPage tb = TabPages[e.TabPageIndex];
                if (tb.Enabled == false)
                    e.Cancel = true;
                else
                    base.OnSelecting(e);
            }
        }
    } 
}
