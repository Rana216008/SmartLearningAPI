public class DashboardViewModel
{
    public int LearnedCount { get; set; }
    public int Attempts { get; set; }
    public int Errors { get; set; }


    public string ActiveMode { get; set; } // هل النظام الآن في وضع امتحان؟
    public string ActiveCategory { get; set; }

  
}
